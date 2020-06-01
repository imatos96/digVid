#include "remote.h"
#include "graph.h"

#define NUM_EVENTS  5

#define NON_STOP    1

/* error codes */
#define NO_ERROR 		0
#define ERROR			1

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
		stringDraw(buffer);
           } 

	
    }
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
