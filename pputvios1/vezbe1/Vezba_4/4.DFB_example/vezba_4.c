#include <stdio.h>
#include <directfb.h>
#include <stdint.h>
#include <linux/input.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

#define NUM_EVENTS  5

#define NON_STOP    1

/* error codes */
#define NO_ERROR 		0
#define ERROR			1

/* helper macro for error checking */
#define DFBCHECK(x...)                                      \
{                                                           \
DFBResult err = x;                                          \
                                                            \
if (err != DFB_OK)                                          \
  {                                                         \
    fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );  \
    DirectFBErrorFatal( #x, err );                          \
  }                                                         \
}

static int32_t inputFileDesc;

int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventRead);
void *pthreadRemoteCntrl();
void inbetweenFunc();

char buffer[20];
static IDirectFBSurface *primary = NULL;
IDirectFB *dfbInterface = NULL;
static int screenWidth = 0;
static int screenHeight = 0;
DFBSurfaceDescription surfaceDesc;

timer_t timerId;
struct sigevent signalEvent;     
struct itimerspec timerSpec;
struct itimerspec timerSpecOld;
int32_t timerFlags = 0;

int32_t main(int32_t argc, char** argv)
{
  
    
   	/* initialize DirectFB */
	DFBCHECK(DirectFBInit(&argc, &argv));
    	/* fetch the DirectFB interface */
	DFBCHECK(DirectFBCreate(&dfbInterface));
    	/* tell the DirectFB to take the full screen for this application */
	DFBCHECK(dfbInterface->SetCooperativeLevel(dfbInterface, DFSCL_FULLSCREEN));
	
    	/* create primary surface with double buffering enabled */
    
	surfaceDesc.flags = DSDESC_CAPS;
	surfaceDesc.caps = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
	DFBCHECK (dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &primary));

	//reći OS-u da notifikaciju šalje pozivanjem specificirane funkcije iz posebne niti
	signalEvent.sigev_notify = SIGEV_THREAD; 
	//funkcija koju će OS prozvati kada interval istekne
	signalEvent.sigev_notify_function = inbetweenFunc; 
	//argumenti funkcije
	signalEvent.sigev_value.sival_ptr = NULL;
	//atributi niti - if NULL default attributes are applied
	signalEvent.sigev_notify_attributes = NULL; 
	timer_create(/*sistemski sat za mjerenje vremena*/ CLOCK_REALTIME,                
             /*podešavanja timer-a*/ &signalEvent,                      
            /*mjesto gdje će se smjestiti ID novog timer-a*/ &timerId);
  
       //brisanje strukture prije setiranja vrijednosti
	memset(&timerSpec,0,sizeof(timerSpec));
	timerSpec.it_value.tv_sec = 3; //3 seconds timeout
	timerSpec.it_value.tv_nsec = 0;
	

        /* create thread for remote */
        pthread_t thread_id;
        printf("Opening thread...");
        pthread_create(&thread_id, NULL, &pthreadRemoteCntrl, NULL);
        //specificiranje vremenskih podešavanja timer-a
	


      pthread_join(thread_id, NULL);
      printf("Nighty night thready...\n");
	/* line drawing */
    
	//DFBCHECK(primary->SetColor(primary, 0xff, 0x80, 0x80, 0xff));
	//DFBCHECK(primary->DrawLine(primary,
                            //   /*x coordinate of the starting point*/ 150,
                            //   /*y coordinate of the starting point*/ screenHeight/3,
                            //   /*x coordinate of the ending point*/screenWidth-200,
                            //   /*y coordinate of the ending point*/ (screenHeight/3)*2));
	
    
	
    
	/* draw image from file */
    
	/*IDirectFBImageProvider *provider;
	IDirectFBSurface *logoSurface = NULL;
	int32_t logoHeight, logoWidth;
	*/
    /* create the image provider for the specified file */
	//DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "dfblogo_alpha.png", &provider));
    /* get surface descriptor for the surface where the image will be rendered */
	//DFBCHECK(provider->GetSurfaceDescription(provider, &surfaceDesc));
    /* create the surface for the image */
	//DFBCHECK(dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &logoSurface));
    /* render the image to the surface */
	//DFBCHECK(provider->RenderTo(provider, logoSurface, NULL));
	
    /* cleanup the provider after rendering the image to the surface */
	//provider->Release(provider);
	
    /* wait 5 seconds before terminating*/
	//sleep(5);
	
    timer_delete(timerId);
    
    //TODO 1: display the keycode of the button on the remote each time any of the buttons are pressed
    //        add background and a frame to the keycode diplay
    //        the displayed keycode should disappear after 3 seconds 
    //TODO 2: add fade-in and fade-out effects
    //TODO 3: add animation - the display should start at the left part of the screen and at the right
    //TODO 4: add support for displaying up to 3 keycodes at the same time, each in a separate row

    return 0;
}
void inbetweenFunc() {
	/* fetch the screen size */
	DFBCHECK (primary->GetSize(primary, &screenWidth, &screenHeight));	

	/* clear the screen before drawing anything (draw black full screen rectangle)*/
	DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
               /*red*/ 0x00,
               /*green*/ 0xf0,
               /*blue*/ 0x0f,
               /*alpha*/ 0xff));

	DFBCHECK(primary->FillRectangle(/*surface to draw on*/ primary,
                    /*upper left x coordinate*/ 0,
                    /*upper left y coordinate*/ 0,
                    /*rectangle width*/ screenWidth,
                    /*rectangle height*/ screenHeight));
	DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
               /*red*/ 0xff,
               /*green*/ 0x0b,
               /*blue*/ 0x00,
               /*alpha*/ 0xff));

	/* switch between the displayed and the work buffer (update the display) */
	DFBCHECK(primary->Flip(primary,
		 /*region to be updated, NULL for the whole surface*/NULL,
	         /*flip flags*/0));

}

void *pthreadRemoteCntrl()
{

    const char* dev = "/dev/input/event0";
    char deviceName[20];
    struct input_event* eventBuf;
    uint32_t eventCnt;
    uint32_t i;
    uint32_t exit=0;
    uint32_t volume=0;
    
    inputFileDesc = open(dev, O_RDWR);
    if(inputFileDesc == -1)
    {
        printf("Error while opening device (%s) !", strerror(errno));
	   // return ERROR;
    }
    
    ioctl(inputFileDesc, EVIOCGNAME(sizeof(deviceName)), deviceName);
	printf("RC device opened succesfully [%s]\n", deviceName);
    
    eventBuf = malloc(NUM_EVENTS * sizeof(struct input_event));
    if(!eventBuf)
    {
        printf("Error allocating memory !");
        //return ERROR;
    }
	
       
    
    while(exit != 1)
    {
        /* read input eventS */
        if(getKeys(NUM_EVENTS, (uint8_t*)eventBuf, &eventCnt))
        {
			printf("Error while reading input events !");
			//return ERROR;
	}

        for(i = 0; i < eventCnt; i++)
        {
           // printf("Event time: %d sec, %d usec\n",(int)eventBuf[i].time.tv_sec,(int)eventBuf[i].time.tv_usec);
           // printf("Event type: %hu\n",eventBuf[i].type);
           // printf("Event code: %hu\n",eventBuf[i].code);
           // printf("Event value: %d\n",eventBuf[i].value);
           // printf("\n");

	  
           // sprintf(*keyBuffer, "%d", eventBuf[i].code);
	   // screenClear(keyBuffer);

	    if(eventBuf[i].code==102)
            {
		exit=1;
    	    }

	    if(eventBuf[i].code==63 && (eventBuf[i].value == 1 || eventBuf[i].value == 2))
	    {
		if(volume<100){
		volume+=5;
		}
		else volume=100;
		printf("Volume: %d\n",volume); //ispisuje se 2 puta kada pustim gumb i stisnem
		
		
            }
            if(eventBuf[i].code==64 && (eventBuf[i].value == 1 || eventBuf[i].value == 2))
	    {
		if(volume>=5){
		volume-=5;
		}
		else volume=0;
		printf("Volume: %d\n",volume); //ispisuje se 2 puta kada pustim gumb i stisnem
            }

	if(eventBuf[i].value == 1)
	{
					
		sprintf(buffer, "KeyCode: %u", eventBuf[i].code);
		timer_settime(timerId,0,&timerSpec,&timerSpecOld);
		/* fetch the screen size */
		DFBCHECK (primary->GetSize(primary, &screenWidth, &screenHeight));	

	       /* clear the screen before drawing anything (draw black full screen rectangle)*/
		DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
               /*red*/ 0x00,
               /*green*/ 0xf0,
               /*blue*/ 0x0f,
               /*alpha*/ 0xff));

		DFBCHECK(primary->FillRectangle(/*surface to draw on*/ primary,
                    /*upper left x coordinate*/ 0,
                    /*upper left y coordinate*/ 0,
                    /*rectangle width*/ screenWidth,
                    /*rectangle height*/ screenHeight));



		DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
               /*red*/ 0xff,
               /*green*/ 0x0b,
               /*blue*/ 0x00,
               /*alpha*/ 0xff));

		/* draw text */

		IDirectFBFont *fontInterface = NULL;
		DFBFontDescription fontDesc;

	/* specify the height of the font by raising the appropriate flag and setting the height value */
		fontDesc.flags = DFDESC_HEIGHT;
		fontDesc.height = 48;

		/* create the font and set the created font for primary surface text drawing */
		DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
		DFBCHECK(primary->SetFont(primary, fontInterface));

		/* draw the text */
		DFBCHECK(primary->DrawString(primary,
				                     /*text to be drawn*/ buffer,
				                     /*number of bytes in the string, -1 for NULL terminated strings*/ -1,
				                     /*x coordinate of the lower left corner of the resulting text*/ 100,
				                     /*y coordinate of the lower left corner of the resulting text*/ 100,
				                     /*in case of multiple lines, allign text to left*/ DSTF_LEFT));


		/* switch between the displayed and the work buffer (update the display) */
		DFBCHECK(primary->Flip(primary,
				               /*region to be updated, NULL for the whole surface*/NULL,
				               /*flip flags*/0));

        } 
	}

	
    }
    
    /*clean up*/ //ostaje u mainu nakon threada
    
	primary->Release(primary);
	dfbInterface->Release(dfbInterface);
    free(eventBuf);
    
    return; // NO_ERROR;

}


int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventsRead)
{
    int32_t ret = 0;
    
    /* read input events and put them in buffer */
    ret = read(inputFileDesc, buf, (size_t)(count * (int)sizeof(struct input_event)));
    if(ret <= 0)
    {
        printf("Error code %d", ret);
        return ERROR;
    }
    /* calculate number of read events */
    *eventsRead = ret / (int)sizeof(struct input_event);
    
    return NO_ERROR;
}

