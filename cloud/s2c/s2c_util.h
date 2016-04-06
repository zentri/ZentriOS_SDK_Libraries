/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once

#include "zos_common.h"
#include "utilities/zos_json.h"
#include "event/zos_events.h"


typedef struct
{
    const char* name;
    zos_event_handler_t event_handler;
    s2c_stream_listener_t listener;
    zos_result_t (*set_enabled)(void *arg, zos_bool_t enabled);
    void *arg;
} s2c_stream_t;

/**
 *
 * @param filename
 * @param json_context
 * @return
 */
zos_result_t s2c_capabilities_parse_file(const char *filename, json_parse_context_t **json_context_ptr);


