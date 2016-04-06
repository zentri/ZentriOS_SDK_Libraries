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




#define S2C_FILE_TARGET(node, number)       ((uint8_t)(number << 4) | (uint8_t)node)
#define S2C_FILE_TARGET_CLOUD               S2C_FILE_TARGET(S2C_NODE_CLOUD, 0)
#define S2C_FILE_TARGET_DMS                 S2C_FILE_TARGET(S2C_NODE_DMS, 0)
#define S2C_FILE_TARGET_MOBILE(num)         S2C_FILE_TARGET(S2C_NODE_MOBILE, num)

/**
 *
 */
typedef struct s2c_file
{
    struct s2c_file *next;
    const char *filename;
    uint32_t size;
    uint32_t crc;
} s2c_file_t;

typedef enum
{
    S2C_FILE_FLAG_OVERWRITE     = (1 << 1)
} s2c_file_flag_t;



/**
 *
 * @param target
 * @param filename
 * @param handle_ptr
 * @return
 * @zos{level=1}
 */
zos_result_t s2c_file_open(uint8_t target, const char *filename, uint32_t *handle_ptr);

/**
 *
 * @param target
 * @param file
 * @param handle_ptr
 * @return
 * @zos{level=1}
 */
zos_result_t s2c_file_create(uint8_t target, const s2c_file_t *file, s2c_file_flag_t flags, uint32_t *handle_ptr);

/**
 *
 * @param handle
 * @param data
 * @param max_size
 * @param bytes_read
 * @return
 * @zos{level=1}
 */
zos_result_t s2c_file_read(uint32_t handle, void *data, uint32_t max_size, uint32_t *bytes_read);

/**
 *
 * @param handle
 * @param data
 * @param size
 * @return
 * @zos{level=1}
 */
zos_result_t s2c_file_write(uint32_t handle, void *data, uint32_t size);

/**
 *
 * @param handle
 * @return
 * @zos{level=1}
 */
zos_result_t s2c_file_close(uint32_t handle, uint32_t timeout);

/**
 *
 * @param target
 * @param filename
 * @param file_info_ptr
 * @return
 */
zos_result_t s2c_file_stat(uint8_t target, const char *filename, s2c_file_t **file_info_ptr);


/**
 *
 * @param target
 * @param filename
 * @return
 */
zos_result_t s2c_file_delete(uint8_t target, const char *filename);


/**
 *
 * @param target
 * @param filename
 * @return
 */
zos_result_t s2c_file_list(uint8_t target, const char *filter, s2c_file_t **file_list_ptr);

/**
 *
 * @param list
 * @zos{level=1,alias=zn_free_linked_list}
 */
void s2c_file_list_destroy(s2c_file_t *list);
