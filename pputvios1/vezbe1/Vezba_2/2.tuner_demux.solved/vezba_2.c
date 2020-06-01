#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "errno.h"
#include "tdp_api.h"
static inline void textColor(int32_t attr, int32_t fg, int32_t bg)
{
   char command[13];

   /* Command is the control command to the terminal */
   sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
   printf("%s", command);
}

#define ASSERT_TDP_RESULT(x,y)  if(NO_ERROR == x) \
                                    printf("%s success\n", y); \
                                else{ \
                                    textColor(1,1,0); \
                                    printf("%s fail\n", y); \
                                    textColor(0,7,0); \
                                    return -1; \
                                }

int32_t myPrivateTunerStatusCallback(t_LockStatus status);
int32_t mySecFilterCallback(uint8_t *buffer);
pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

struct PROGRAM {
 uint16_t program_number;
 uint16_t pids;
};

struct PAT_HEADER{
 uint8_t table_id;
 uint8_t section_syntax_indicator;
 uint8_t zero;
 uint8_t reserved;
 uint16_t section_length;
 uint16_t transport_stream_id;
 uint8_t version_number;
 uint8_t section_number;
 uint8_t current_next_indicator;
 uint8_t last_section_number;
 struct PROGRAM *program;
 uint32_t crc;
 uint8_t cnt;
};
struct PAT_HEADER pat_header;
 

int32_t main()
{
    int32_t result;
    
    uint32_t playerHandle = 0;
    uint32_t sourceHandle = 0;
    uint32_t filterHandle = 0;
    uint32_t streamHandleAudio = 0;
    uint32_t streamHandleVideo = 0;
    
    
    struct timespec lockStatusWaitTime;
    struct timeval now;
    
    gettimeofday(&now,NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec+10;
    
    /* Initialize tuner */
    result = Tuner_Init();
    ASSERT_TDP_RESULT(result, "Tuner_Init");
    
    /* Register tuner status callback */
    result = Tuner_Register_Status_Callback(myPrivateTunerStatusCallback);
    ASSERT_TDP_RESULT(result, "Tuner_Register_Status_Callback");
    
    /* Lock to frequency */
    result = Tuner_Lock_To_Frequency(818000000, 8, DVB_T);
    ASSERT_TDP_RESULT(result, "Tuner_Lock_To_Frequency");
    
    pthread_mutex_lock(&statusMutex);
    if(ETIMEDOUT == pthread_cond_timedwait(&statusCondition, &statusMutex, &lockStatusWaitTime))
    {
        printf("\n\nLock timeout exceeded!\n\n");
        return -1;
    }
    pthread_mutex_unlock(&statusMutex);
    
    /* Initialize player (demux is a part of player) */
    result = Player_Init(&playerHandle);
    ASSERT_TDP_RESULT(result, "Player_Init");
    
    /* Open source (open data flow between tuner and demux) */
    result = Player_Source_Open(playerHandle, &sourceHandle);
    ASSERT_TDP_RESULT(result, "Player_Source_Open");
    printf("i am here1");
    /* Open stream video  */
    result = Player_Stream_Create(playerHandle, sourceHandle, 101, VIDEO_TYPE_MPEG2, &streamHandleVideo);
    ASSERT_TDP_RESULT(result, "Player_Stream_Create");
    printf("i am here2");
   /* Open stream audio  */
    result = Player_Stream_Create(playerHandle, sourceHandle, 103, AUDIO_TYPE_MPEG_AUDIO, &streamHandleAudio);
   ASSERT_TDP_RESULT(result, "Player_Stream_Create");
     printf("i am here3");
    /* Set filter to demux */
    result = Demux_Set_Filter(playerHandle, 0x0000, 0x00, &filterHandle);
    ASSERT_TDP_RESULT(result, "Demux_Set_Filter");
    
    /* Register section filter callback */
    result = Demux_Register_Section_Filter_Callback(mySecFilterCallback);
    ASSERT_TDP_RESULT(result, "Demux_Register_Section_Filter_Callback");
    
    /* Wait for a while to receive several PAT sections */
    fflush(stdin);
    getchar();
    
    /* Deinitialization */
    
    /* Free demux filter */
    result = Demux_Free_Filter(playerHandle, filterHandle);
    ASSERT_TDP_RESULT(result, "Demux_Free_Filter");
    
    /* Close previously opened source */
    result = Player_Source_Close(playerHandle, sourceHandle);
    ASSERT_TDP_RESULT(result, "Player_Source_Close");
    
    /* Deinit player */
    result = Player_Deinit(playerHandle);
    ASSERT_TDP_RESULT(result, "Player_Deinit");
    
    /* Deinit tuner */
    result = Tuner_Deinit();
    ASSERT_TDP_RESULT(result, "Tuner_Deinit");
    
    return 0;
}

int32_t myPrivateTunerStatusCallback(t_LockStatus status)
{
    if(status == STATUS_LOCKED)
    {
        pthread_mutex_lock(&statusMutex);
        pthread_cond_signal(&statusCondition);
        pthread_mutex_unlock(&statusMutex);
        printf("\n\n\tCALLBACK LOCKED\n\n");
    }
    else
    {
        printf("\n\n\tCALLBACK NOT LOCKED\n\n");
    }
    return 0;
}

int32_t mySecFilterCallback(uint8_t *buffer)
{
    //printf("\n\nSection arrived!!!\n\n");
    pat_header.table_id=(*buffer);
    pat_header.section_syntax_indicator = (uint8_t)(*(buffer+1) >> 7 & 0b00000001);
    //pat_header.zero = (uint8_t) (*(buffer+2) & 0b10000000);
    //pat_header.reserved = (uint8_t) (*(buffer+3) & 0b10001000);
    pat_header.section_length = (uint16_t) (((*(buffer+1) << 8) + *(buffer+2)) & 0x0FFF);
    pat_header.transport_stream_id = (uint16_t) (((*(buffer+3) << 8 + *(buffer+4))));	
    pat_header.version_number = (uint8_t) (((*buffer+5) >> 1) & 0b00000001);
    pat_header.current_next_indicator = (uint8_t) ((*buffer+5) & 0b10000000);
    pat_header.section_number = (uint8_t) (*buffer+6);
    pat_header.last_section_number = (uint8_t) (*buffer+7);
    
    pat_header.cnt= (pat_header.section_length-9)/4;
    int progStart = (*buffer+8);
    pat_header.program = malloc (pat_header.cnt * sizeof (pat_header.program));

    int i=0;
    for (i=0; i<pat_header.cnt;i++)
    {
	pat_header.program[i].program_number = (uint16_t) ((*(buffer+8+(i*4)) << 8) + *(buffer+8+1+(i*4)));
 	//printf ("\nprogram_number: %d", pat_header.program[i].program_number);
        if(pat_header.program[i].program_number != 0)
	{
	pat_header.program[i].pids = (uint16_t) (((*(buffer+10+(i*4)) & 0b00011111) << 8) + *(buffer+11+(i*4)));

	//printf ("\tpids: %d", pat_header.program[i].pids);
	}
    }

    pat_header.crc = (uint32_t) ((*(buffer+8+pat_header.cnt*4) << 24) + (*(buffer+9+pat_header.cnt*4) << 16) + (*(buffer+10+pat_header.cnt*4) << 8) + (*(buffer+11+pat_header.cnt*4)));    

    /*printf ("\nTable ID: %d", pat_header.table_id);
    printf ("\nsection_syntax_indicator: %d", pat_header.section_syntax_indicator);
    printf ("\nZero: %d", pat_header.zero);
    printf ("\nReserved: %d", pat_header.reserved);
    printf ("\nSection length: %d", pat_header.section_length);
    printf("\nTS ID: %d", pat_header.transport_stream_id);
    printf ("\nVerNum: %d", pat_header.version_number);
    printf ("\nCurrent_next_indicator: %d", pat_header.current_next_indicator);
    printf ("\nlast_section_number: %d", pat_header.last_section_number);
    printf ("\nCRC: %d", pat_header.crc);
    free(pat_header.program);
    //callback_status=1;*/
    return 0;
}
