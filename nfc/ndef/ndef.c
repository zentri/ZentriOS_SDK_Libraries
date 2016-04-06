/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "nfc.h"


/*************************************************************************************************/
zos_result_t nfc_create_record(ndef_record_t **record_ptr, ndef_header_flag_t flags, ndef_record_type_t type, const uint8_t *buffer, uint8_t length)
{
    zos_result_t result;
    ndef_record_t *record;

    if(!ZOS_FAILED(result, zn_malloc((uint8_t**)&record, sizeof(ndef_record_t) + length)))
    {
        record->header.flags = flags;
        record->header.length = length;
        record->header.type = type;
        record->header.type_length = 1;
        memcpy(record->payload, buffer, length);
        *record_ptr = record;
    }
    return result;
}

/*************************************************************************************************/
zos_result_t nfc_create_text_record(ndef_text_record_t **record_ptr, const char *text, const char *language)
{
    zos_result_t result;
    ndef_text_record_t *record;
    const uint8_t lang_len = strlen(language);
    const uint8_t payload_len = 1 + lang_len + strlen(text);

    if(!ZOS_FAILED(result, zn_malloc((uint8_t**)&record, sizeof(ndef_text_record_t) + payload_len + 1)))
    {
        record->header.flags = NDEF_TNF_WELL_KNOWN|NDEF_FLAG_SHORT_RECORD|NDEF_FLAG_END_MSG|NDEF_FLAG_BEGIN_MSG;
        record->header.length = payload_len;
        record->header.type = NDEF_RECORD_TYPE_TEXT;
        record->header.type_length = 1;
        record->status = lang_len;

        record->lang = &record->payload[0];
        strcpy((char*)record->lang, language);
        record->text = (char*)&record->payload[1 + lang_len + 1];
        strcpy(record->text, text);

        *record_ptr = record;
    }
    return result;
}

/*************************************************************************************************/
zos_result_t nfc_create_uri_record(ndef_uri_record_t **record_ptr, const char *uri, ndef_uri_code_t code)
{
    zos_result_t result;
    ndef_uri_record_t *record;
    const uint8_t payload_len = 1 + strlen(uri);

    if(!ZOS_FAILED(result, zn_malloc((uint8_t**)&record, sizeof(ndef_uri_record_t) + payload_len)))
    {
        record->header.flags = NDEF_TNF_WELL_KNOWN|NDEF_FLAG_SHORT_RECORD|NDEF_FLAG_END_MSG|NDEF_FLAG_BEGIN_MSG;
        record->header.length = payload_len;
        record->header.type = NDEF_RECORD_TYPE_URI;
        record->header.type_length = 1;
        record->code = code;
        strcpy((char*)record->uri, uri);

        *record_ptr = record;
    }
    return result;
}
