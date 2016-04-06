/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "zos.h"

extern char *strtok_r(char *str, const char *delim, char **saveptr);


#define FILE_TARGET_NODE_MASK 0x0F
#define FILE_TARGET_NODE(target) (target & FILE_TARGET_NODE_MASK)
#define FILE_TARGET_MOBILE_NUM(target) ((uint8_t)target >> 4)



/*************************************************************************************************/
zos_result_t s2c_file_stat(uint8_t target, const char *filename, s2c_file_t **file_info_ptr)
{
    *file_info_ptr = NULL;

    if(FILE_TARGET_NODE(target) == S2C_NODE_MOBILE)
    {
        zos_result_t result;
        char res_buffer[128];

        *file_info_ptr = NULL;

        if(!ZOS_FAILED(result, s2c_command_issue_return_str(FILE_TARGET_MOBILE_NUM(target),
                                                            res_buffer, sizeof(res_buffer), "s2cfst %s", filename)))
        {
            s2c_file_t *file_info;
            char *crc_idx = strchr(res_buffer, ',');
            if(crc_idx == NULL)
            {
                return ZOS_BAD_RESPONSE;
            }

            *crc_idx++ = 0;

            if(ZOS_FAILED(result, zn_malloc((uint8_t**)&file_info, sizeof(s2c_file_t) + strlen(filename) + 1)))
            {
                return result;
            }

            file_info->filename = (const char*)&file_info[1];
            strcpy((char*)file_info->filename, filename);
            file_info->size = str_to_uint32(res_buffer);
            file_info->crc = str_hex_to_uint32(crc_idx);

            *file_info_ptr = file_info;
        }

        return result;
    }
    else if(FILE_TARGET_NODE(target) == S2C_NODE_CLOUD)
    {
        // TODO add cloud FS support
        return ZOS_UNIMPLEMENTED;
    }
    else
    {
        return ZOS_INVALID_ARG;
    }
}

/*************************************************************************************************/
zos_result_t s2c_file_delete(uint8_t target, const char *filename)
{
    if(FILE_TARGET_NODE(target) == S2C_NODE_MOBILE)
    {
        return s2c_command_issue(FILE_TARGET_MOBILE_NUM(target), "s2cfde %s", filename);
    }
    else if(FILE_TARGET_NODE(target) == S2C_NODE_CLOUD)
    {
        // TODO add cloud FS support
        return ZOS_UNIMPLEMENTED;
    }
    else
    {
        return ZOS_INVALID_ARG;
    }
}


/*************************************************************************************************/
zos_result_t s2c_file_list(uint8_t target, const char *filter, s2c_file_t **file_list_ptr)
{
    s2c_file_t *file_list = NULL;
    *file_list_ptr = NULL;

    if(FILE_TARGET_NODE(target) == S2C_NODE_CLOUD)
    {
        // TODO add cloud FS support
        return ZOS_UNIMPLEMENTED;
    }
    else if(FILE_TARGET_NODE(target) == S2C_NODE_MOBILE)
    {
#define BUFFER_LEN 2048
        zos_result_t result;
        int handle = 0;
        char *buffer = NULL;

        if(filter == NULL)
        {
            filter = "";
        }

        if(ZOS_FAILED(result, zn_malloc((uint8_t**)&buffer, BUFFER_LEN)))
        {
            goto exit;
        }

        for(;;)
        {
            if(ZOS_FAILED(result, s2c_command_issue_return_str(FILE_TARGET_MOBILE_NUM(target), buffer, BUFFER_LEN, "s2cfls %d 16 %s", handle, filter)))
            {
                goto exit;
            }
            else
            {
                char *next_line;
                if(*buffer == 0)
                {
                    goto exit;
                }

                int count = 0;
                for(char *line = buffer; (line = strtok_r(line, "\r\n", &next_line)) != NULL; line = next_line, ++count)
                {
                    s2c_file_t *file;
                    char *next_tok;
                    char *toks[4];
                    int i = 0;

                    for(char *tok = line; i < 4 && (tok = strtok_r(tok, ",", &next_tok)) != NULL; tok = next_tok, ++i)
                    {
                        toks[i] = tok;
                    }

                    if(ZOS_FAILED(result, zn_malloc((uint8_t**)&file, sizeof(s2c_file_t) + strlen(toks[0]))))
                    {
                        goto exit;
                    }

                    file->filename = (char*)&file[1];
                    strcpy((char*)file->filename, toks[0]);
                    file->size = str_to_uint32(toks[1]);
                    file->crc = str_hex_to_uint32(toks[2]);
                    handle = str_to_uint32(toks[3]) + 1;
                    file->next = file_list;
                    file_list = file;
                }

                if(count < 16)
                {
                    break;
                }
            }
        }

        exit:
        if(buffer != NULL)
        {
            free(buffer);
        }
        if(result == ZOS_SUCCESS)
        {
            *file_list_ptr = file_list;
        }
        else
        {
            s2c_file_list_destroy(file_list);
        }

        return result;
    }
    else
    {
        return ZOS_INVALID_ARG;
    }
}
