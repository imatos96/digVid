#ifndef _MAIN_H_
#define _MAIN_H_

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


#define DESIRED_FREQUENCY 818000000	        /* Tune frequency in Hz */
#define BANDWIDTH 8    				/* Bandwidth in Mhz */
#define VIDEO_PID 101				/* Channel video pid */
#define AUDIO_PID 103				/* Channel audio pid */

static int32_t inputFileDesc;
timer_t timerId;
struct sigevent signalEvent;     
struct itimerspec timerSpec;
struct itimerspec timerSpecOld;
int32_t timerFlags = 0;

#endif
