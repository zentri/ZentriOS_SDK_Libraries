/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once

#include "zos_common.h"


#ifndef S2C_COMMAND_TIMEOUT
#define S2C_COMMAND_TIMEOUT 7000 // milliseconds
#endif



/**
 *
 * @param mobile_num
 * @param cmd
 * @return
 */
zos_result_t s2c_command_issue(uint8_t mobile_num, const char *cmd, ...);

/**
 *
 * @param mobile_num
 * @param data
 * @param data_len
 * @param cmd
 * @return
 */
zos_result_t s2c_command_issue_with_data(uint8_t mobile_num, const void *data, uint16_t data_len, const char *cmd, ...);

/**
 *
 * @param mobile_num
 * @param buffer
 * @param buffer_len_ptr
 * @param cmd
 * @return
 */
zos_result_t s2c_command_issue_return_data(uint8_t mobile_num, void *buffer, uint16_t *buffer_len_ptr, const char *cmd, ...);

/**
 *
 * @param mobile_num
 * @param retval_ptr
 * @param cmd
 * @return
 */
zos_result_t s2c_command_issue_return_uint32(uint8_t mobile_num, uint32_t *retval_ptr, const char *cmd, ...);

/**
 *
 * @param mobile_num
 * @param str_buffer
 * @param max_len
 * @param cmd
 * @return
 */
zos_result_t s2c_command_issue_return_str(uint8_t mobile_num, char *str_buffer, uint16_t max_len, const char *cmd, ...);






/******************************************************
 * The following is used internally by the S2C library
 ******************************************************/


typedef struct
{
    const uint8_t *data;
    uint8_t *response;
    uint16_t data_len;
    uint16_t response_len;
    uint32_t timeout;
    uint16_t result_code;
} s2c_command_config_t;


/**
 * @zos{level=1,alias=device_command_issue_to_mobile}
 */
zos_result_t s2c_command_issue_with_vargs(uint8_t mobile_num, s2c_command_config_t *config, const char *fmt, va_list args);
