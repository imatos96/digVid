#include "graph.h"

//missing draw info functionconnected to pmt table source - give an argument of prm from global structure in main - small black rectangle with string in 

void inbetweenFunc() {
	/* fetch the screen size */
	DFBCHECK (primary->GetSize(primary, &screenWidth, &screenHeight));	

	/* clear the screen before drawing anything (draw black full screen rectangle)*/
	DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
	       /*red*/ 0x00,
	       /*green*/ 0x00,
	       /*blue*/ 0x00,
	       /*alpha*/ 0x00));

	DFBCHECK(primary->FillRectangle(/*surface to draw on*/ primary,
               /*upper left x coordinate*/ 0,
               /*upper left y coordinate*/ 0,
               /*rectangle width*/ screenWidth,
               /*rectangle height*/ screenHeight));
	DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
               /*red*/ 0x00,
               /*green*/ 0x0b,
               /*blue*/ 0x00,
               /*alpha*/ 0x00));

	/* switch between the displayed and the work buffer (update the display) */
	DFBCHECK(primary->Flip(primary,
	       /*region to be updated, NULL for the whole surface*/NULL,
	       /*flip flags*/0));

}

void initialize(){
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

}

void stringDraw(char[2000] buffer){
       /* fetch the screen size */ 
       DFBCHECK (primary->GetSize(primary, &screenWidth, &screenHeight));	

       /* clear the screen before drawing anything (draw black full screen rectangle)*/
       DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
               /*red*/ 0x00,
               /*green*/ 0x00,
               /*blue*/ 0x00,
               /*alpha*/ 0x00);

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
       FBFontDescription fontDesc;

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

void drawImage(char[100] imagePath){

      /* draw image from file */
    
       IDirectFBImageProvider *provider;
       IDirectFBSurface *logoSurface = NULL;
       int32_t logoHeight, logoWidth;
	
     /* create the image provider for the specified file */
       DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, imagePath, &provider));
     /* get surface descriptor for the surface where the image will be rendered */
       DFBCHECK(provider->GetSurfaceDescription(provider, &surfaceDesc));
     /* create the surface for the image */
       DFBCHECK(dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &logoSurface));
     /* render the image to the surface */
       DFBCHECK(provider->RenderTo(provider, logoSurface, NULL));
	
     /* cleanup the provider after rendering the image to the surface */
       provider->Release(provider);
	
}

void cleanup(){
      /*clean up*/ //ostaje u mainu nakon threada
    
      primary->Release(primary);
      dfbInterface->Release(dfbInterface);
}



