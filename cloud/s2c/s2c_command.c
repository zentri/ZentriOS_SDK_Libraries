/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "zos.h"



static zos_result_t issue_command(uint8_t mobile_num, const void *src, void *dst, uint16_t *len_ptr, const char *cmd, va_list args);


/*************************************************************************************************/
zos_result_t s2c_command_issue(uint8_t mobile_num, const char *cmd, ...)
{
    zos_result_t result;
    va_list args;
    va_start(args, cmd);
    result =  issue_command(mobile_num, NULL, NULL, NULL, cmd, args);
    va_end(args);

    return result;
}

/*************************************************************************************************/
zos_result_t s2c_command_issue_with_data(uint8_t mobile_num, const void *data, uint16_t data_len, const char *cmd, ...)
{
    zos_result_t result;
    va_list args;
    va_start(args, cmd);
    result =  issue_command(mobile_num, data, NULL, &data_len, cmd, args);
    va_end(args);

    return result;
}

/*************************************************************************************************/
zos_result_t s2c_command_issue_return_data(uint8_t mobile_num, void *buffer, uint16_t *buffer_len_ptr, const char *cmd, ...)
{
    zos_result_t result;
    va_list args;
    va_start(args, cmd);
    result =  issue_command(mobile_num, NULL, buffer, buffer_len_ptr, cmd, args);
    va_end(args);

    return result;
}

/*************************************************************************************************/
zos_result_t s2c_command_issue_return_uint32(uint8_t mobile_num, uint32_t *retval_ptr, const char *cmd, ...)
{
    zos_result_t result;
    va_list args;
    char buffer[16];
    uint16_t len = sizeof(buffer)-1;

    va_start(args, cmd);
    result =  issue_command(mobile_num, NULL, buffer, &len, cmd, args);
    va_end(args);

    if(result == ZOS_SUCCESS)
    {
        buffer[len] = 0;
        *retval_ptr = str_to_uint32(buffer);
    }

    return result;
}

/*************************************************************************************************/
zos_result_t s2c_command_issue_return_str(uint8_t mobile_num, char *str_buffer, uint16_t max_len, const char *cmd, ...)
{
    zos_result_t result;

    va_list args;
    va_start(args, cmd);
    result =  issue_command(mobile_num, NULL, str_buffer, &max_len, cmd, args);
    va_end(args);

    str_buffer[max_len] = 0;

    return result;
}


/*************************************************************************************************/
static zos_result_t issue_command(uint8_t mobile_num, const void *src, void *dst, uint16_t *len_ptr, const char *cmd, va_list args)
{
    zos_result_t result;
    uint16_t dummy;

    if(len_ptr == NULL)
    {
        len_ptr = &dummy;
    }

    s2c_command_config_t config =
    {
            .data = src,
            .response = dst,
            .response_len = (dst != NULL) ? *len_ptr : 0,
            .data_len = (src != NULL) ? *len_ptr : 0,
            .timeout = S2C_COMMAND_TIMEOUT
    };

    result = s2c_command_issue_with_vargs(mobile_num, &config, cmd, args);

    if(result == ZOS_SUCCESS)
    {
        *len_ptr = config.response_len;

        if(config.result_code != CMD_SUCCESS)
        {
            result = ZOS_CMD_CODES_START + config.result_code;
        }
    }
    else
    {
        *len_ptr = 0;
    }

    return result;
}
