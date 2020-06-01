#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "errno.h"
#include "tdp_api.h"

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

struct PMT_HEADER{
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

struct PAT_HEADER pmt_header;
struct PAT_HEADER pat_header;


int32_t myPrivateTunerStatusCallback(t_LockStatus status);
int32_t mySecFilterCallback(uint8_t *buffer);
pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

int32_t myPrivateTunerStatusCallback(t_LockStatus status);
int32_t table_parser_PAT(uint8_t *buffer);
//int32_t table_parser_PMT(uint8_t *buffer);
