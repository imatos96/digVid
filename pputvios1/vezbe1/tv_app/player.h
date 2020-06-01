#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "tdp_api.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define DESIRED_FREQUENCY 666000000		/* Tune frequency in Hz */
#define BANDWIDTH 8    				/* Bandwidth in Mhz */
#define VIDEO_PID 101				/* Channel video pid */
#define AUDIO_PID 103				/* Channel audio pid */

#define ASSERT_TDP_RESULT(x,y)  if(NO_ERROR == x) \
                                    printf("%s success\n", y); \
                                else{ \
                                    textColor(1,1,0); \
                                    printf("%s fail\n", y); \
                                    textColor(0,7,0); \
                                    return -1; \
                                }

int32_t result;
uint32_t playerHandle = 0;
uint32_t sourceHandle = 0;
uint32_t filterHandle = 0;
uint32_t streamHandleAudio = 0;
uint32_t streamHandleVideo = 0;

static int32_t tunerStatusCallback(t_LockStatus status);

int32_t initializePlayer();
int32_t streamAV();
int32_t deinitializePlayer();


#endif
