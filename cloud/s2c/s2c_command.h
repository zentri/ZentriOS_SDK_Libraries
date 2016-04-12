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


/**
 * @addtogroup lib_cloud_s2c_macros
 * @{
 */

#ifndef S2C_COMMAND_TIMEOUT

/**
 * The default command response timeout. This is the maximum amount of time in milliseconds
 * the Device should wait for a command response from a Mobile.
 * Define this value in the global defines to override its value
 */
#define S2C_COMMAND_TIMEOUT 7000 // milliseconds

#endif

/**
 * @}
 * @addtogroup lib_cloud_s2c_types
 * @{
 */

/**
 * @brief Command response code
 */
typedef enum
{
    S2C_RESPONSE_CODE_SUCCESS           = 0, //!< The command was successful
    S2C_RESPONSE_CODE_FAILED            = 1, //!< The command failed, probably in the firmware somewhere
    S2C_RESPONSE_CODE_PARSE_ERROR       = 2, //!< Failed to parse the command
    S2C_RESPONSE_CODE_UNKNOWN           = 3, //!< Unknown command
    S2C_RESPONSE_CODE_TOO_FEW_ARGS      = 4, //!< Not enough command arguments
    S2C_RESPONSE_CODE_TOO_MANY_ARGS     = 5, //!< Too many command arguments
    S2C_RESPONSE_CODE_UNKNOWN_OPTION    = 6, //!< Unknown option (e.g. known 'set' command option)
    S2C_RESPONSE_CODE_BAD_ARGS          = 7, //!< Invalid command arguments
    S2C_RESPONSE_CODE_BUFFER_OVERFLOW   = 8, //!< Serial command line buffer overflow
} s2c_response_code_t;

/**
 * @}
 * @addtogroup lib_cloud_s2c_cmd
 * @{
 */

/**
 * @brief Issue a command to a locally connected Mobile
 *
 *
 * @param mobile_num The mobile client number
 * @param cmd Formatted command string
 * @return Result of API call, see @ref zos_result_t
 */
zos_result_t s2c_command_issue(uint8_t mobile_num, const char *cmd, ...);

/**
 * @brief Issue a command with additional data to a locally connected Mobile
 *
 * Like @ref s2c_command_issue() this issues a command to a Mobile, however it also
 * supplies additional data after the command string.
 *
 * An example usage would be the `write` command
 * @code
 * write 25
 * <25 bytes of data ...>
 * @endcode
 *
 * The 'cmd' arg would be the 'write 25', the 'data' arg would be a pointer to buffer holding 25 bytes of data.
 *
 * @param mobile_num The mobile client number
 * @param data Pointer to buffer holding command data
 * @param data_len Length of data buffer
 * @param cmd Formatted command string
 * @return Result of API call, see @ref zos_result_t
 */
zos_result_t s2c_command_issue_with_data(uint8_t mobile_num, const void *data, uint16_t data_len, const char *cmd, ...);

/**
 * @brief Issue command to Mobile, return binary data from response
 *
 * Like @ref s2c_command_issue() this issues a command to a Mobile, however
 * it also returns data from the command's response.
 *
 * An example usage would be the `read` command
 * @code
 * read 25
 * <25 bytes of data ...>
 * @endcode
 *
 * The 'cmd' arg would be the 'read 25', the 'buffer' arg would be a pointer to buffer to hold 25 bytes of response data from the Mobile.
 *
 *
 * @note The buffer_len_ptr argument should point to a uint16_t variable containing the size of the supplied buffer. Upon
 *       returning the buffer_len_ptr argument contains the actual response data size received from the Mobile.
 *
 * @param mobile_num The mobile client number
 * @param buffer Buffer to hold command response data
 * @param buffer_len_ptr Pointer containing size of supplied buffer, upon returning contains actual response size from Mobile
 * @param cmd Formatted command string
 * @return Result of API call, see @ref zos_result_t
 */
zos_result_t s2c_command_issue_return_data(uint8_t mobile_num, void *buffer, uint16_t *buffer_len_ptr, const char *cmd, ...);

/**
 * @brief Issue command to Mobile, return uint32 response value
 *
 * @param mobile_num The mobile client number
 * @param retval_ptr Pointer to uint32 to hold response value
 * @param cmd Formatted command string
 * @return Result of API call, see @ref zos_result_t
 */
zos_result_t s2c_command_issue_return_uint32(uint8_t mobile_num, uint32_t *retval_ptr, const char *cmd, ...);

/**
 * @brief Issue command to Mobile, return null-terminated string response.
 *
 * @param mobile_num The mobile client number
 * @param str_buffer Buffer to hold string response
 * @param max_len Maximum length of response string (i.e. size of string buffer)
 * @param cmd Formatted command string
 * @return Result of API call, see @ref zos_result_t
 */
zos_result_t s2c_command_issue_return_str(uint8_t mobile_num, char *str_buffer, uint16_t max_len, const char *cmd, ...);


/**
 * @}
 */




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
