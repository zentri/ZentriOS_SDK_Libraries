/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once

#include "ftp.h"


#define FTP_CONTEXT_ID 0x12345678
#define FTP_PROCESSING_PERIOD 100


#ifdef FTP_DEBUG_ENABLED
#define FTP_DEBUG(msg, ...) ZOS_LOG("FTP: "msg, ## __VA_ARGS__)
#else
#define FTP_DEBUG(msg, ...)
#endif



typedef struct
{
    uint32_t id;
    uint32_t control_handle;
    uint32_t data_handle;
    uint16_t passive_port;
    ftp_upload_config_t config;
    ftp_state_t state;
    ftp_state_t old_state;
    uint32_t timestamp;
    uint8_t rx_status;
    char rx_buffer[128];
} internal_ftp_context_t;

extern internal_ftp_context_t *internal_context;



zos_bool_t ftp_context_is_valid(internal_ftp_context_t *context);
void ftp_processing_handler(void * arg);
void ftp_receive_event_handler(uint32_t handle);
void send_ftp_command(internal_ftp_context_t *context, const char*fmt, ...);
