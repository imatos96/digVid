#include "graph.h"

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
