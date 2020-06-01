#ifndef _GRAPH_H_
#define _GRAPH_H_
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

void inbetweenFunc();
void initialize();
int32_t stringDraw(char buffer[2000]);
int32_t drawImage(char imagePath[100]);
void cleanup();

#endif
