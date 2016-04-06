/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once


#include "nfc.h"


typedef enum
{
    NFC_DRIVER_SRAM,
    NFC_DRIVER_EEPROM
} nfc_driver_memory_t;


zos_result_t nfc_driver_init(const nfc_config_t *config);

zos_result_t nfc_driver_nvm_erase_all(zos_bool_t add_empty_record);

zos_result_t nfc_driver_add_tlv_block(nfc_driver_memory_t memory, uint8_t *buffer, uint16_t length);

zos_result_t nfc_driver_read_tlv_block(nfc_driver_memory_t memory, uint8_t *block_ptr, uint8_t *block_offset_ptr, uint8_t *buffer);

zos_result_t nfc_driver_set_pass_through_enabled(nfc_direction_t direction, zos_gpio_t gpio, zos_gpio_irq_callback_t callback);

zos_result_t nfc_driver_read_bytes(uint32_t block, uint32_t offset, void *buffer, uint32_t length);

zos_result_t nfc_driver_write_bytes(uint32_t block, uint32_t offset, const void *buffer, uint32_t length);

zos_result_t nfc_driver_set_last_ndef_block(uint8_t block_addr);

zos_result_t nfc_driver_poll_status(nfc_status_t *status);


zos_result_t nfc_driver_find_next_tlv_record(nfc_driver_memory_t memory, uint8_t *block_ptr, uint8_t *block_offset_ptr, tlv_block_type_t needle);

void nfc_driver_get_memory_range(nfc_driver_memory_t memory, uint8_t *start_ptr, uint8_t *stop_ptr);
