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

#include "ndef.h"


typedef enum
{
    NFC_NVM_READ        = (1 << 0),
    NFC_DATA_RECEIVED   = (1 << 1),
    NFC_DATA_TX_READY   = (1 << 2),
    NFC_RF_DETECTED     = (1 << 3)
} nfc_status_t;


typedef void (*nfc_event_handler_t)(nfc_status_t event);

typedef struct
{
    struct
    {
        zos_gpio_t gpio;
        nfc_event_handler_t handler;
        nfc_status_t status_flags;
    } event;
} nfc_config_t;


typedef enum
{
    NFC_NONE,
    NFC_BUS_TO_RF,
    NFC_RF_TO_BUS
} nfc_direction_t;

typedef struct
{
    ndef_record_type_t record_type;
    const char *str;
} nfc_search_parameters_t;



#define nfc_free_records(record_list) zn_free_linked_list(record_list)


zos_result_t nfc_init(const nfc_config_t *config);

zos_result_t nfc_nvm_erase_all_records(zos_bool_t add_empty_record);

zos_result_t nfc_nvm_add_record(const ndef_record_t *record);

zos_result_t nfc_nvm_set_all_records_added(void);

zos_result_t nfc_nvm_read_records(ndef_record_t **record_list);

zos_result_t nfc_nvm_find_records(ndef_record_t **record_list, const nfc_search_parameters_t *params);



zos_result_t nfc_set_pass_through_enabled(nfc_direction_t direction);

zos_result_t nfc_read_records(ndef_record_t **record_list);

zos_result_t nfc_write_records(ndef_record_t *records);

zos_result_t nfc_read_bytes(uint32_t block, uint32_t offset, void *buffer, uint32_t length);

zos_result_t nfc_write_bytes(uint32_t block, uint32_t offset, const void *buffer, uint32_t length);

zos_result_t nfc_poll_status(nfc_status_t *status);
