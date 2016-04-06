/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "zos.h"


static zos_result_t caps_file_reader(void *user, void *data, uint32_t max_length, uint32_t *bytes_read);



/*************************************************************************************************/
zos_result_t s2c_capabilities_parse_file(const char *filename, json_parse_context_t **json_context_ptr)
{
    zos_result_t result;
    uint32_t handle = ZOS_INVALID_HANDLE;
    char buffer[256];
    json_parse_config_t config =
    {
            .buffer = buffer,
            .buffer_len = sizeof(buffer),
            .reader = caps_file_reader
    };

    json_parse_context_init(json_context_ptr, &config);

    if(ZOS_FAILED(result, zn_file_open(filename, &handle)))
    {
    }
    else if(ZOS_FAILED(result, json_parse_chunked(*json_context_ptr, (void*)handle)))
    {
    }

    zn_file_close(handle);

    return result;
}


/*************************************************************************************************/
static zos_result_t caps_file_reader(void *user, void *data, uint32_t max_length, uint32_t *bytes_read)
{
    return zn_file_read((uint32_t)user, data, max_length, bytes_read);
}
