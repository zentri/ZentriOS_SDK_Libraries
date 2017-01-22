/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once


#include "arducam_types.h"



#define REG_ADDR_ACTION 0x00
#define REG_ACTION_TERMINATE 0xFF



typedef struct
{
    struct
    {
        uint32_t rate;
        uint32_t flags;
    } spi;
    struct
    {
        uint32_t rate;
        uint16_t address;
        uint8_t retries;
        uint8_t read_timeout;
    } i2c;
    struct
    {
        zos_result_t (*init)(const arducam_driver_config_t *config);
        zos_result_t (*start_capture)(void);
        zos_result_t (*stop_capture)(void);
        zos_result_t (*reset)(void);
        zos_result_t (*is_capture_ready)(uint32_t *image_size_ptr);
        zos_result_t (*read_data)(void *data, uint16_t length, uint16_t *remaining_ptr);
        zos_result_t (*set_setting)(arducam_setting_type_t setting, uint32_t value);
        zos_result_t (*get_setting)(arducam_setting_type_t setting, uint32_t *value_ptr);
    } callback;

} adrucam_driver_t;


#pragma pack(1)
typedef struct
{
    const uint8_t address;
    const uint8_t value;
} reg_addr_value_t;
#pragma pack()


zos_result_t adrucam_get_driver(arducam_type_t type, const adrucam_driver_t **driver_ptr);

zos_result_t adrucam_driver_init(zos_spi_device_t *spi, zos_i2c_device_t *i2c, adrucam_driver_t *driver, const arducam_driver_config_t *config);

uint8_t adrucam_driver_i2c_write_reg(uint16_t addr, uint8_t data);

uint8_t adrucam_driver_i2c_read_reg(uint16_t addr, uint8_t *val);

zos_result_t adrucam_driver_i2c_write_regs(const reg_addr_value_t *regs, const reg_addr_value_t *action_list, uint8_t action_list_len);

zos_result_t adrucam_driver_i2c_write_reg_list(const uint8_t *list, uint16_t length);
