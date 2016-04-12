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

/**
 * @addtogroup lib_cloud_s2c_macros
 * @{
 */


#define S2C_FILE_TARGET(_node, _num) (s2c_file_target_t){ .node = _node, .mobile_num = _num }

/**
 * Helper macro to create a @ref S2C_NODE_CLOUD @ref s2c_file_target_t
 */
#define S2C_FILE_TARGET_CLOUD               S2C_FILE_TARGET(S2C_NODE_CLOUD, 0)

/**
 * Helper macro to create a @ref S2C_NODE_DMS_DEVICE @ref s2c_file_target_t
 */
#define S2C_FILE_TARGET_DMS_DEVICE         S2C_FILE_TARGET(S2C_NODE_DMS_DEVICE, 0)

/**
 * Helper macro to create a @ref S2C_NODE_DMS_PRODUCT @ref s2c_file_target_t
 */
#define S2C_FILE_TARGET_DMS_PRODUCT         S2C_FILE_TARGET(S2C_NODE_DMS_PRODUCT, 0)

/**
 * Helper macro to create a @ref S2C_NODE_MOBILE @ref s2c_file_target_t
 */
#define S2C_FILE_TARGET_MOBILE(num)         S2C_FILE_TARGET(S2C_NODE_MOBILE, num)

/**
 * The maximum file name/path length in bytes
 */
#define S2C_FILENAME_MAX_LEN 96

/**
 * @}
 * @addtogroup lib_cloud_s2c_types
 * @{
 */


/**
 * Target of S2C file operation
 */
typedef struct
{
    s2c_node_t node;            ///< Target node of file operation, @ref s2c_node_t
    uint8_t mobile_num;         ///< If node is @ref S2C_NODE_MOBILE , this must be populated with the mobile number
} s2c_file_target_t;


/**
 * S2C file information
 */
typedef struct s2c_file
{
    struct s2c_file *next;      ///< Used internally for linked-list operation
    const char *filename;       ///< Name of file
    uint32_t size;              ///< Size in bytes of file
    uint32_t crc;               ///< Checksum of file
    uint32_t timestamp;         ///< File creation timestamp
} s2c_file_t;


/**
 * S2C file operation flags
 */
typedef enum
{
    S2C_FILE_FLAG_OVERWRITE     = (1 << 1)  //!< Overwrite exiting file if necessary
} s2c_file_flag_t;


/**
 * S2C File handle, returned by @ref s2c_file_open() or @ref s2c_file_create()
 */
typedef struct
{
    s2c_node_t node;
    void *internal;
} s2c_file_handle_t;

/**
 * File listing parameters
 */
typedef struct
{
    const char *filter;         ///< Filename filter, only list files whose name watch the given filter, leave NULL to return all files
    uint32_t offset;            ///< Start index of files to return, optional, leave 0 to start from beginning
    uint32_t limit;             ///< Maximum number of files to return, optional, leave 0 to return all files
} s2c_file_list_parameters_t;


/**
 * @}
 * @addtogroup lib_cloud_s2c_file
 * @{
 */

/**
 *
 * @param host
 * @param cert_filename
 */
void s2c_file_set_cloud_settings(const char *host, const char *cert_filename);

/**
 * @brief Open a file for reading
 *
 * This opens a file on a Mobile client or Cloud, target should be one of the following:
 * - @ref S2C_FILE_TARGET_CLOUD : read file from the S2C cloud
 * - @ref S2C_FILE_TARGET_DMS_DEVICE : Device Management Server (DMS) device
 * - @ref S2C_FILE_TARGET_DMS_PRODUCT : Device Management Server (DMS) device product
 * - @ref S2C_FILE_TARGET_MOBILE : read file from locally connected mobile client
 *
 * If the API returns successfully, a handle is returned. The @ref s2c_file_read()
 * API should be used to read the file's data.
 *
 * Issue the @ref s2c_file_close() API when finished reading the file.
 *
 * @param target The target of the read operation, see @ref s2c_file_target_t
 * @param filename The name of the file on the target to open for reading
 * @param handle_ptr Pointer to hold handle of opened file
 * @return Result of API call, see @ref zos_result_t
 * @zos{level=1}
 */
zos_result_t s2c_file_open(s2c_file_target_t target, s2c_file_t *file, s2c_file_handle_t *handle_ptr);

/**
 * @brief Open a file for writing
 *
 * This creates a file on a Mobile client or Cloud, target should be one of the following:
 * - @ref S2C_FILE_TARGET_CLOUD : create file from the S2C cloud
 * - @ref S2C_FILE_TARGET_DMS_DEVICE : Device Management Server (DMS) device
 * - @ref S2C_FILE_TARGET_MOBILE : create file from locally connected mobile client
 *
 * If the API returns successfully, a handle is returned. The @ref s2c_file_write()
 * API should be used to write the file's data.
 *
 * Issue the @ref s2c_file_close() API when finished writing the file.
 *
 * The @ref s2c_file_t argument should contain information about the file to create including:
 * - Filename
 * - File size in bytes
 * - Optionally the CRC of the file's data. The target will verify the CRC after the file is written. Leave as UINT32_MAX if unused.
 *
 * @param target The target of the write operation, see @ref s2c_file_target_t
 * @param file Information about file to create, see @ref s2c_file_t
 * @param flags File creation flags, see @ref s2c_file_flag_t
 * @param handle_ptr Pointer to hold handle of opened file
 * @return Result of API call, see @ref zos_result_t
 * @zos{level=1}
 */
zos_result_t s2c_file_create(s2c_file_target_t target, const s2c_file_t *file, s2c_file_flag_t flags, s2c_file_handle_t *handle_ptr);

/**
 * @brief Read file data
 *
 * This reads file data of opened file. The file should have been previously opened using @ref s2c_file_open()
 *
 * @param handle Handle of opened file
 * @param data Buffer to hold read file data
 * @param max_size Size of supplied data buffer
 * @param bytes_read Pointer to hold number of bytes actually read
 * @return Result of API call, see @ref zos_result_t
 * @zos{level=1}
 */
zos_result_t s2c_file_read(const s2c_file_handle_t *handle, void *data, uint32_t max_size, uint32_t *bytes_read);

/**
 * @brief Write file data
 *
 * This writes file data of file opened for writing. The file should have been previously opened using @ref s2c_file_create()
 *
 * @param handle Handle of opened file
 * @param data Buffer holding data to write to file
 * @param size Number of bytes to write to file
 * @return Result of API call, see @ref zos_result_t
 * @zos{level=1}
 */
zos_result_t s2c_file_write(const s2c_file_handle_t *handle, void *data, uint32_t size);

/**
 * @brief Close previously opened file
 *
 * This closes a file opened with @ref s2c_file_open() or @ref s2c_file_create()
 *
 * @param handle Handle of opened file.
 * @param timeout_ms Time in milliseconds to wait for target to finish operation.
 * @return Result of API call, see @ref zos_result_t
 * @zos{level=1}
 */
zos_result_t s2c_file_close(const s2c_file_handle_t *handle, uint32_t timeout_ms);

/**
 * @brief Return information about file
 *
 * This returns information about a file on a Mobile client or Cloud, target should be one of the following:
 * - @ref S2C_FILE_TARGET_CLOUD : S2C cloud
 * - @ref S2C_FILE_TARGET_DMS_DEVICE : Device Management Server (DMS) device
 * - @ref S2C_FILE_TARGET_DMS_PRODUCT : Device Management Server (DMS) device product
 * - @ref S2C_FILE_TARGET_MOBILE : locally connected mobile client
 *
 * @note The returned file_info_ptr argument MUST be cleaned using either @ref zn_free() or @ref s2c_file_list_destroy()
 *        otherwise the memory will be leaked.
 *
 * @param target The target of the stat operation, see @ref s2c_file_target_t
 * @param filename Name of file to gain information about
 * @param file_info_ptr Pointer to hold allocated @ref s2c_file_t containing information about file
 * @return Result of API call, see @ref zos_result_t
 * @zos{level=1}
 */
zos_result_t s2c_file_stat(s2c_file_target_t target, const char *filename, s2c_file_t *file_info);


/**
 * @brief Delete file on Mobile or Cloud
 *
 * This deletes a file on a Mobile client or Cloud, target should be one of the following:
 * - @ref S2C_FILE_TARGET_CLOUD : S2C cloud
 * - @ref S2C_FILE_TARGET_DMS_DEVICE : Device Management Server (DMS) device
 * - @ref S2C_FILE_TARGET_MOBILE : locally connected mobile client
 *
 * @param target The target of the delete operation, see @ref s2c_file_target_t
 * @param filename Name of file to delete
 * @return Result of API call, see @ref zos_result_t
 * @zos{level=1}
 */
zos_result_t s2c_file_delete(s2c_file_target_t target, const char *filename);


/**
 * @brief List files on Mobile or Cloud
 *
 * This lists files on a Mobile client or Cloud, target should be one of the following:
 * - @ref S2C_FILE_TARGET_CLOUD : S2C cloud
 * - @ref S2C_FILE_TARGET_DMS_DEVICE : Device Management Server (DMS) device
 * - @ref S2C_FILE_TARGET_DMS_PRODUCT : Device Management Server (DMS) device product
 * - @ref S2C_FILE_TARGET_MOBILE : locally connected mobile client
 *
 * Optionally a filter can be supplied to limit the file listing.
 *
 * @note @ref s2c_file_list_destroy() MUST be used to cleanup the returned file listing.
 *
 * @param target The target of the list operation, see @ref s2c_file_target_t
 * @param filter Optional pattern to filter file names/paths. The wildcard character '*' can be used at the beginning of the pattern. Leave NULL to list all files
 * @param file_list_ptr Pointer to hold linked-list of @ref s2c_file_t containing file listing
 * @return Result of API call, see @ref zos_result_t
 * @zos{level=1}
 */
zos_result_t s2c_file_list(s2c_file_target_t target, const s2c_file_list_parameters_t *parameters, s2c_file_t **file_list_ptr);

/**
 * @brief Destroy linked-list
 *
 * This frees the memory allocated by the file listing returned by @ref s2c_file_list()
 *
 * @param list File listing to cleanup
 * @zos{level=1,alias=zn_free_linked_list}
 */
void s2c_file_list_destroy(s2c_file_t *list);

/**
 * @brief Upload local file to cloud or Mobile client
 *
 * This uploads a local file to the Mobile client or Cloud, target should be one of the following:
 * - @ref S2C_FILE_TARGET_CLOUD : S2C cloud
 * - @ref S2C_FILE_TARGET_DMS_DEVICE : Device Management Server (DMS) device
 * - @ref S2C_FILE_TARGET_MOBILE : locally connected mobile client
 *
 * @param[in] target Target of file operation
 * @param[in] local_filename Name of file on local file system to upload
 * @param[in] remote_filename Optional, name of created file on remote side, leave NULL to use local_filename as remote_filename
 * @return Result of API, see @ref zos_result_t
 * @zos{level=1}
 */
zos_result_t s2c_file_upload(s2c_file_target_t target, const char *local_filename, const char *remote_filename);

/**
 * @brief Download remote file on cloud or Mobile client to local file system
 *
 * This downloads a remote file on the Mobile client or Cloud to the local file system, target should be one of the following:
 * - @ref S2C_FILE_TARGET_CLOUD : S2C cloud
 * - @ref S2C_FILE_TARGET_DMS_DEVICE : Device Management Server (DMS) device
 * - @ref S2C_FILE_TARGET_DMS_PRODUCT : Device Management Server (DMS) device product
 * - @ref S2C_FILE_TARGET_MOBILE : locally connected mobile client
 *
 * @param[in] target Target of file operation
 * @param[in] remote_filename File name/path of remote file
 * @param[in] local_filename Optional, name of locally created file, leave NULL to use remote_filename as local_filename
 * @return Result of API, see @ref zos_result_t
 * @zos{level=1}
 */
zos_result_t s2c_file_download(s2c_file_target_t target, const char *remote_filename, const char *local_filename);

/**
 * @}
 */
