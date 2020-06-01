#include "player.h"

static inline void textColor(int32_t attr, int32_t fg, int32_t bg)
{
   char command[13];

   /* Command is the control command to the terminal */
   sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
   printf("%s", command);
}


int32_t tunerStatusCallback(t_LockStatus status)
{
    if(status == STATUS_LOCKED)
    {
        pthread_mutex_lock(&statusMutex);
        pthread_cond_signal(&statusCondition);
        pthread_mutex_unlock(&statusMutex);
        printf("\n%s -----TUNER LOCKED-----\n",__FUNCTION__);
    }
    else
    {
        printf("\n%s -----TUNER NOT LOCKED-----\n",__FUNCTION__);
    }
    return 0;
}

void tunerUp(){
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
}
