/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once





typedef struct
{
    zos_semaphore_t wait_sem;
    int argc;
    char **argv;
    zos_cmd_result_t result;
} file_command_context_t;


typedef struct
{
    zos_bool_t network_is_up;
    zos_bool_t cloud_connected;
    zos_bool_t cond_network_connect;
    zos_bool_t cond_cloud_connect;
} network_current_status_t;

