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





#define CHECK_INITIALIZED()


typedef struct
{
    uint8_t tlv_type;
    uint8_t tlv_length;
    uint8_t flags;
    uint8_t type_len;
    uint8_t payload_len;
    uint8_t type;
    uint8_t payload[1];
} tlv_ndef_record_t;




static zos_result_t find_records(nfc_driver_memory_t memory, ndef_record_t **record_list, const nfc_search_parameters_t *params);
static zos_result_t add_record(nfc_driver_memory_t memory, const ndef_record_t *record);
static void nfc_gpio_irq_callback(void *arg);


static nfc_config_t nfc_config;



/*************************************************************************************************/
zos_result_t nfc_init(const nfc_config_t *config)
{
    zos_result_t result;

    memcpy(&nfc_config, config, sizeof(nfc_config_t));

    if(!ZOS_FAILED(result, nfc_driver_init(config)))
    {
        if(config->event.handler != NULL)
        {
            zn_event_enable_irq_events(8);
            nfc_set_pass_through_enabled(NFC_NONE);
        }
        nfc_nvm_set_all_records_added();
    }

    return result;
}

/*************************************************************************************************/
zos_result_t nfc_nvm_erase_all_records(zos_bool_t add_empty_record)
{
    CHECK_INITIALIZED();
    return nfc_driver_nvm_erase_all(add_empty_record);
}


/*************************************************************************************************/
zos_result_t nfc_nvm_add_record(const ndef_record_t *record)
{
    return add_record(NFC_DRIVER_EEPROM, record);
}

/*************************************************************************************************/
zos_result_t nfc_nvm_set_all_records_added(void)
{
    zos_result_t result;
    uint8_t block = 0;
    uint8_t offset = 0;

    if(ZOS_FAILED(result, nfc_driver_find_next_tlv_record(NFC_DRIVER_EEPROM, &block, &offset, TLV_TYPE_TERMINATOR)))
    {
        if(result != ZOS_NOT_FOUND)
        {
            return result;
        }
        block = 0;
        offset = 0;
    }

    return nfc_driver_set_last_ndef_block(block);
}

/*************************************************************************************************/
zos_result_t nfc_nvm_read_records(ndef_record_t **record_list)
{
    return find_records(NFC_DRIVER_EEPROM, record_list, NULL);
}

/*************************************************************************************************/
zos_result_t nfc_read_records(ndef_record_t **record_ptr)
{
    return find_records(NFC_DRIVER_SRAM, record_ptr, NULL);
}

/*************************************************************************************************/
zos_result_t nfc_write_records(ndef_record_t *record)
{
    return add_record(NFC_DRIVER_SRAM, record);
}

/*************************************************************************************************/
zos_result_t nfc_nvm_find_records(ndef_record_t **record_list, const nfc_search_parameters_t *params)
{
    return find_records(NFC_DRIVER_EEPROM, record_list, params);
}

/*************************************************************************************************/
zos_result_t nfc_set_pass_through_enabled(nfc_direction_t direction)
{
    return nfc_driver_set_pass_through_enabled(direction, nfc_config.event.gpio, nfc_gpio_irq_callback);
}

/*************************************************************************************************/
zos_result_t nfc_read_bytes(uint32_t block, uint32_t offset, void *buffer, uint32_t length)
{
    return nfc_driver_read_bytes(block, offset, buffer, length);
}

/*************************************************************************************************/
zos_result_t nfc_write_bytes(uint32_t block, uint32_t offset, const void *buffer, uint32_t length)
{
    return nfc_driver_write_bytes(block, offset, buffer, length);
}

/*************************************************************************************************/
zos_result_t nfc_poll_status(nfc_status_t *status)
{
    return nfc_driver_poll_status(status);
}


/*************************************************************************************************/
static zos_result_t add_record(nfc_driver_memory_t memory, const ndef_record_t *record)
{
    zos_result_t result;
    uint8_t *buffer;
    const uint8_t tot_len = TLV_HEADER_LEN + NDEF_HEADER_LEN + record->header.length + TLV_TERMINATOR_LEN;

    CHECK_INITIALIZED();

    if(!ZOS_FAILED(result, zn_malloc(&buffer, tot_len)))
    {
        tlv_ndef_record_t *rec = (tlv_ndef_record_t*)buffer;

        // TLV block header
        rec->tlv_type = NDEF_TLV_TYPE;                      // Field Type (0x03 = NDEF Message)
        rec->tlv_length = tot_len - TLV_HEADER_LEN - TLV_TERMINATOR_LEN;

        // NDEF Message Header
        rec->flags = (uint8_t)record->header.flags;
        rec->type_len = 1; // the **Type Length** for the Record Type Indicator, currently one 1 byte length supported
        rec->payload_len = record->header.length; // the payload length
        rec->type = (uint8_t)record->header.type; // Record Type Indicator

        // NDEF message payload
        memcpy(rec->payload, record->payload, record->header.length);

        // TLV block terminator
        buffer[tot_len-1] = (uint8_t)TLV_TYPE_TERMINATOR;

        result = nfc_driver_add_tlv_block(memory, buffer, tot_len);

        zn_free(buffer);
    }

    return result;
}


/*************************************************************************************************/
static zos_result_t find_records(nfc_driver_memory_t memory, ndef_record_t **record_list, const nfc_search_parameters_t *params)
{
    zos_result_t result = ZOS_SUCCESS;
    uint8_t block_num = 0;
    uint8_t offset = 0;
    uint8_t *buffer;
    ndef_record_t *head_record = NULL;

    if(!ZOS_FAILED(result, zn_malloc(&buffer, NDEF_TLV_MAX_PAYLOAD+TLV_HEADER_LEN)))
    {
        for(;;)
        {
            if(ZOS_FAILED(result, nfc_driver_read_tlv_block(memory, &block_num, &offset, buffer)))
            {
                break;
            }
            const tlv_ndef_record_t *rec = (tlv_ndef_record_t*)buffer;
            if((rec->flags != (NDEF_TNF_WELL_KNOWN|NDEF_FLAG_SHORT_RECORD|NDEF_FLAG_END_MSG|NDEF_FLAG_BEGIN_MSG)) ||
               (rec->type_len != 1))
            {
                continue;
            }
            else if(params != NULL)
            {
                if(params->record_type != rec->type)
                {
                    continue;
                }
                else if(params->str != NULL && rec->type == NDEF_RECORD_TYPE_TEXT)
                {
                    const char *text = (const char*)&rec->payload[1 + (rec->payload[0] & NDEF_TEXT_LANG_LEN_MASK)];
                    if(strstr((char*)params->str, text) == NULL)
                    {
                        continue;
                    }
                }
                else if(params->str != NULL && rec->type == NDEF_RECORD_TYPE_URI)
                {
                    const char *uri = (const char*)&rec->payload[1];
                    if(strstr((char*)params->str, uri) == NULL)
                    {
                        continue;
                    }
                }
            }

            if(rec->type == NDEF_RECORD_TYPE_TEXT)
            {
                ndef_text_record_t *text_rec;
                const uint8_t tot_len = sizeof(ndef_text_record_t) + rec->payload_len - 1 + 2; // -1 to remove status field, +2 for string null terminators
                if(ZOS_FAILED(result, zn_malloc((uint8_t**)&text_rec, tot_len)))
                {
                    break;
                }
                const uint8_t lang_len = (rec->payload[0] & NDEF_TEXT_LANG_LEN_MASK);
                text_rec->status = rec->payload[0];
                text_rec->lang = &text_rec->payload[0];
                memcpy(text_rec->lang, &rec->payload[1], lang_len);
                text_rec->text = (char*)&text_rec->payload[lang_len + 1];
                memcpy(text_rec->text, &rec->payload[1 + lang_len], rec->payload_len - 1 - lang_len);
                text_rec->next = head_record;
                head_record = (ndef_record_t*)text_rec;
            }
            else if(rec->type == NDEF_RECORD_TYPE_URI)
            {
                ndef_uri_record_t *uri_rec;
                const uint8_t tot_len = sizeof(ndef_uri_record_t) + rec->payload_len -1 + 1; // -1 to remove fieldd code, +1 for null terminator
                if(ZOS_FAILED(result, zn_malloc((uint8_t**)&uri_rec, tot_len)))
                {
                    break;
                }
                uri_rec->code = rec->payload[0];
                memcpy(uri_rec->uri, &rec->payload[1], rec->payload_len-1);
                uri_rec->next = head_record;
                head_record = (ndef_record_t*)uri_rec;
            }
        }

       zn_free(buffer);
    }

    if(result != ZOS_SUCCESS)
    {
        if(result != ZOS_NOT_FOUND)
        {
            nfc_free_records(head_record);
            head_record = NULL;
        }
        else if(head_record != NULL)
        {
            result = ZOS_SUCCESS;
        }
    }

    *record_list = head_record;

    return result;
}

/*************************************************************************************************/
static void nfc_gpio_event_handler(void *arg)
{
    if(nfc_config.event.handler != NULL)
    {
         nfc_status_t status;
         if(nfc_poll_status(&status) == ZOS_SUCCESS)
         {
             if(status & nfc_config.event.status_flags)
             {
                 nfc_config.event.handler(status);
             }
         }
    }
}

/*************************************************************************************************/
static void nfc_gpio_irq_callback(void *arg)
{
    zn_event_issue(nfc_gpio_event_handler, NULL, EVENT_FLAGS1(FROM_IRQ));
}


