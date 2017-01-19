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
        zos_result_t (*init)(const arducam_settings_t *config);
        zos_result_t (*start_capture)(void);
        zos_result_t (*stop_capture)(void);
        zos_result_t (*is_capture_ready)(uint32_t *image_size_ptr);
        zos_result_t (*read_data)(void *data, uint16_t length);
        zos_result_t (*set_setting)(arducam_setting_type_t setting, uint32_t value);
        zos_result_t (*get_setting)(arducam_setting_type_t setting, uint32_t *value_ptr);
    } callback;

} adrucam_driver_t;


zos_result_t adrucam_get_driver(arducam_type_t type, const adrucam_driver_t **driver_ptr);
