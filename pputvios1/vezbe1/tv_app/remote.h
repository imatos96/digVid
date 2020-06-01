#ifndef _REMOTE_H_
#define _REMOTE_H


#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>

#define NUM_EVENTS  5

#define NON_STOP    1

/* error codes */
#define NO_ERROR 		0
#define ERROR			1


static int32_t inputFileDesc;

int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventRead);
void *pthreadRemoteCntrl();
