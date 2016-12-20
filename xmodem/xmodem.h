/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once


#include "zos.h"


typedef void (*xmodem_read_error_t)(zos_result_t error);


typedef struct
{
    struct
    {
        zos_event_handler_t block_read;
        xmodem_read_error_t read_error;
    } callback;
    zos_uart_t uart;
    uint16_t idle_period;
} xmodem_config_t;



zos_result_t xmodem_init(const xmodem_config_t *config);

zos_result_t xmodem_read_start(void);

zos_result_t xmodem_read_stop(void);

zos_result_t xmodem_read_halt(void);

zos_result_t xmodem_write(void *data, uint32_t length, zos_bool_t eot);
