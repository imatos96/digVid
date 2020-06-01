#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "errno.h"
#include "tdp_api.h"
#include "parser.h"

int32_t table_parser_PAT(uint8_t *buffer)
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

/*
int32_t table_parser_PMT(uint8_t *buffer) {
	uint8_t *section_data = pmt->section_header->data;
	int section_len = pmt->section_header->data_len;

	pmt->reserved1         =  (section_data[0] &~ 0x1F) >> 5;						// xxx11111
	pmt->PCR_pid           = ((section_data[0] &~ 0xE0) << 8) | section_data[1];	// 111xxxxx xxxxxxxx

	pmt->reserved2         =  (section_data[2] &~ 0x0F) >> 4;						// xxxx1111
	pmt->program_info_size = ((section_data[2] &~ 0xF0) << 8) | section_data[3];	// 1111xxxx xxxxxxxx

	/* Handle streams */
	/*uint8_t *stream_data = section_data + 4 + pmt->program_info_size;	// +4 is to compensate for reserved1,PCR,reserved2,program_info_size
	int stream_len       = section_len - pmt->program_info_size - 4;		// -4 for the CRC at the end

	pmt->program_info = NULL;
	if (pmt->program_info_size) {
		pmt->program_info = malloc(pmt->program_info_size);
		if (pmt->program_info) {
			memcpy(pmt->program_info, stream_data - pmt->program_info_size, pmt->program_info_size);
		}
	}

	while (stream_len > 0) {
		if (pmt->streams_num == pmt->streams_max) {
			ts_LOGf("PMT contains too many streams (>%d), not all are initialized!\n", pmt->streams_max);
			break;
		}

		struct ts_pmt_stream *sinfo = calloc(1, sizeof(struct ts_pmt_stream));

		sinfo->stream_type  = stream_data[0];

		sinfo->reserved1    =  (stream_data[1] &~ 0x1F) >> 5;					// xxx11111
		sinfo->pid          = ((stream_data[1] &~ 0xE0) << 8) | stream_data[2];	// 111xxxxx xxxxxxxx

		sinfo->reserved2    =  (stream_data[3] &~ 0x0F) >> 4;					// xxxx1111
		sinfo->ES_info_size = ((stream_data[3] &~ 0xF0) << 8) | stream_data[4];	// 1111xxxx xxxxxxxx

		sinfo->ES_info      = NULL;
		if (sinfo->ES_info_size > 0) {
			sinfo->ES_info = malloc(sinfo->ES_info_size);
			memcpy(sinfo->ES_info, &stream_data[5], sinfo->ES_info_size);
		}
		pmt->streams[pmt->streams_num] = sinfo;
		pmt->streams_num++;

		stream_data += 5 + sinfo->ES_info_size;
		stream_len  -= 5 + sinfo->ES_info_size;
	}

	if (!ts_crc32_section_check(pmt->section_header, "PMT"))
		return 0;

	pmt->initialized = 1;
	return 1;
}*/
