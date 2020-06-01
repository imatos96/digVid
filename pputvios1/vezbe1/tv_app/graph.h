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
