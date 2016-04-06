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


#define Ht16K33_BRIGHTNESS_MAX      15
#define HT16K33_BLINKRATE_OFF       0x00
#define HT16K33_BLINKRATE_2HZ       0x01
#define HT16K33_BLINKRATE_1HZ       0x02
#define HT16K33_BLINKRATE_HALFHZ    0x03



zos_result_t ht16k33_init(zos_i2c_t port, zos_i2c_device_t *device);

void ht16k33_deinit(const zos_i2c_device_t *device);

zos_result_t ht16k33_set_brightness(const zos_i2c_device_t *devicee, uint8_t brightness);

zos_result_t ht16k33_set_blink_rate(const zos_i2c_device_t *device, uint8_t rate);

zos_result_t ht16k33_write_display(const zos_i2c_device_t *device, uint16_t *buffer);

zos_result_t ht16k33_clear_display(const zos_i2c_device_t *device);
