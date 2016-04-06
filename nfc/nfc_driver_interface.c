/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "nfc.h"
#include "nfc_driver_interface.h"





/*************************************************************************************************/
zos_result_t nfc_driver_find_next_tlv_record(nfc_driver_memory_t memory, uint8_t *block_ptr, uint8_t *block_offset_ptr, tlv_block_type_t needle)
{
    zos_result_t result;
    uint8_t buffer[NDEF_BLOCK_SIZE];
    int16_t length = 0;
    uint8_t block = *block_ptr;
    uint8_t offset = *block_offset_ptr;
    uint8_t start_block = 0;
    uint8_t stop_block = 0;

    nfc_driver_get_memory_range(memory, &start_block, &stop_block);
    block += start_block;

    for(;;)
    {
        if(block > stop_block)
        {
           if(needle == TLV_TYPE_TERMINATOR)
           {
        	   block = start_block;
        	   offset = 0;
        	   result = ZOS_SUCCESS;
           }
           else
           {
        	   result = ZOS_NOT_FOUND;
           }
           goto exit;
        }
        else if(ZOS_FAILED(result, nfc_driver_read_bytes(block, 0, buffer, NDEF_BLOCK_SIZE)))
        {
            break;
        }

        for(;offset < NDEF_BLOCK_SIZE; ++offset)
        {
            if(length == 0)
            {
                const tlv_block_type_t block_type = (tlv_block_type_t)buffer[offset];
                if(block_type == TLV_TYPE_TERMINATOR)
                {
                    result = (needle == TLV_TYPE_TERMINATOR) ? ZOS_SUCCESS : ZOS_NOT_FOUND;
                    goto exit;
                }
                else if(block_type == TLV_TYPE_NULL)
                {
                    length = 1;
                }
                else if(block_type == TLV_TYPE_NDEF)
                {
                    if(needle == TLV_TYPE_NDEF)
                    {
                        goto exit;
                    }
                    length = -1;
                }
                else if(block_type == TLV_TYPE_PROPRIETARY)
                {
                    length = -1;
                }
                else
                {
                    result = ZOS_ERROR;
                    goto exit;
                }
            }
            else if(length > 0)
            {
                --length;
            }
            else
            {
                length = buffer[offset];

                // 3 byte lengths not currently supported
                if(length == 0xFF)
                {
                    result = ZOS_UNSUPPORTED;
                    goto exit;
                }
            }
        }

        offset = 0;
        ++block;
    }

exit:
    *block_ptr = block;
    *block_offset_ptr = offset;

    return result;
}
