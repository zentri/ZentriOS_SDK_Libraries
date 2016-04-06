/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once


#include "zos.h"


/**
 * @addtogroup lib_protocols_ftp_types
 * @{
 */

/**
 * @brief Current FTP state
 */
typedef enum
{
    FTP_STATE_IDLE   = 0, //!< FTP_STATE_IDLE
    FTP_STATE_INIT   = 1, //!< FTP_STATE_INIT
    FTP_STATE_OPEN   = 2, //!< FTP_STATE_OPEN
    FTP_STATE_LOGIN  = 3, //!< FTP_STATE_LOGIN
    FTP_STATE_USER   = 4, //!< FTP_STATE_USER
    FTP_STATE_PASS   = 5, //!< FTP_STATE_PASS
    FTP_STATE_CWD    = 6, //!< FTP_STATE_CWD
    FTP_STATE_TYPE   = 7, //!< FTP_STATE_TYPE
    FTP_STATE_LISTEN = 8, //!< FTP_STATE_LISTEN
    FTP_STATE_ASK    = 9, //!< FTP_STATE_ASK
    FTP_STATE_STOR   = 10,//!< FTP_STATE_STOR
    FTP_STATE_DATA   = 11,//!< FTP_STATE_DATA
    FTP_STATE_QUIT   = 12,//!< FTP_STATE_QUIT
    FTP_STATE_CLOSE  = 13,//!< FTP_STATE_CLOSE
    FTP_STATE_END    = 14,//!< FTP_STATE_END
    FTP_STATE_FAIL   = 15,//!< FTP_STATE_FAIL
    FTP_STATE_COUNT       //!< FTP_STATE_COUNT
} ftp_state_t;



/**
 * @brief FTP state change callback
 * @param new_state New state of FTP state-machine, see @ref ftp_state_t
 * @return @ref ZOS_SUCCESS if FTP should continue processing, anything else is an error and FTP should stop processing
 */
typedef zos_result_t (*ftp_state_change_func_t)(ftp_state_t new_state);


/**
 * @brief FTP upload configuration, used by @ref ftp_async_upload()
 */
typedef struct
{
    const char *host;                   /// FTP hostname
    uint16_t port;                      /// FTP port, 21 for non-secure
    const char* username;               /// FTP username
    const char *password;               /// FTP password
    const char *directory;              /// Directory on FTP server to upload file, leave NULL to upload to root
    const char *filename;               /// Name of file to create on remote FTP server
    uint32_t timeout;                   /// Maximum time in milliseconds to wait for an FTP operation before terminating
    ftp_state_change_func_t state_callback_callback; /// Callback to be issued when FTP state changes, see @ref ftp_state_change_func_t
} ftp_upload_config_t;

/**
 * Generic FTP context object
 */
typedef void* ftp_context_t;

/**
 * @}
 * @addtogroup lib_protocols_ftp
 * @{
 */

/**
 * @brief Beging asynchronous FTP file upload
 *
 * This begins a FTP file upload. The actual data upload begins when the @ref ftp_state_t
 * is @ref FTP_STATE_DATA . In this state, @ref ftp_async_upload_write() should be called
 * as many times as needed to upload data to remote file on FTP server.
 *
 * Once all data is written, call @ref ftp_async_upload_write() one more times with a NULL buffer to
 * complete the upload.
 *
 * @note @ref ftp_async_upload_cleanup() should be called once upload is complete
 *
 * @note To view library debug messages, define FTP_DEBUG_ENABLED as a global variable.
 *
 * @param context_ptr Pointer to hold allocated @ref ftp_context_t
 * @param config Configuration of upload, see @ref ftp_upload_config_t
 * @return Result of call, see @ref zos_result_t
 */
zos_result_t ftp_async_upload(ftp_context_t **context_ptr, const ftp_upload_config_t *config);

/**
 * @brief Write FTP data to upload to remote server
 *
 * See @ref ftp_async_upload() for more information.
 *
 * @param context Upload context allocated by @ref ftp_async_upload()
 * @param data Data to write to remote FTP file, set as NULL to complete upload
 * @param length Length of data in bytes
 * @return Result of call, see @ref zos_result_t
 */
zos_result_t ftp_async_upload_write(ftp_context_t *context, const void *data, uint16_t length);

/**
 * @brief Cleanup upload context allocated by @ref ftp_async_upload()
 *
 * @param context Upload context allocated by @ref ftp_async_upload()
 */
void ftp_async_upload_cleanup(ftp_context_t *context);

/**
 * @brief Return FTP state as string
 * @param state FTP state, see @ref ftp_state_t
 * @return FTP state as string
 */
const char* ftp_get_state_str(ftp_state_t state);

/**
 * @}
 */


