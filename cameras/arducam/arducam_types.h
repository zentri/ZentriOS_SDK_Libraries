/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once


#include "zos.h"



#define _ARDUCAM_LEVEL_COUNT(name) (((ARDUCAM_ ## name ## _LEVEL_MAX) - (ARDUCAM_## name ##_LEVEL_MIN)) + 1)

#define ARDUCAM_BRIGHTNESS_LEVEL_MIN     -2
#define ARDUCAM_BRIGHTNESS_LEVEL_MAX     2
#define ARDUCAM_BRIGHTNESS_LEVELS        _ARDUCAM_LEVEL_COUNT(BRIGHTNESS)

#define ARDUCAM_CONTRAST_LEVEL_MIN       -2
#define ARDUCAM_CONTRAST_LEVEL_MAX       2
#define ARDUCAM_CONTRAST_LEVELS          _ARDUCAM_LEVEL_COUNT(CONTRAST)

#define ARDUCAM_SATURATION_LEVEL_MIN     -2
#define ARDUCAM_SATURATION_LEVEL_MAX     2
#define ARDUCAM_SATURATION_LEVELS        _ARDUCAM_LEVEL_COUNT(SATURATION)

#define ARDUCAM_SPECIALEFFECT_DISABLE     -1
#define ARDUCAM_SPECIALEFFECT_NEGATIVE    0x00
#define ARDUCAM_SPECIALEFFECT_GRAY        0x01
#define ARDUCAM_SPECIALEFFECT_SEPIA       0x02
#define ARDUCAM_SPECIALEFFECT_BLUISH      0x03
#define ARDUCAM_SPECIALEFFECT_REDDISH     0x04
#define ARDUCAM_SPECIALEFFECT_GREENISH    0x05
#define ARDUCAM_SPECIALEFFECT_ALLFIXED    0x06
#define ARDUCAM_SPECIALEFFECT_MAX         ARDUCAM_SPECIALEFFECT_ALLFIXED

#define ARDUCAM_ZOOM_MAX                 22

#define ARDUCAM_QUALITY_MIN 0
#define ARDUCAM_QUALITY_MAX 10

#define ARDUCAM_GAINCEILING_MIN 0
#define ARDUCAM_GAINCEILING_MAX 6




typedef enum
{
    ARDUCAM_SETTING_RESOLUTION,
    ARDUCAM_SETTING_BRIGHTNESS,
    ARDUCAM_SETTING_CONTRAST,
    ARDUCAM_SETTING_SATURATION,
    ARDUCAM_SETTING_GAINCEILING,
    ARDUCAM_SETTING_QUALITY,
    ARDUCAM_SETTING_MIRROR,
    ARDUCAM_SETTING_FLIP,
    ARDUCAM_SETTING_FORMAT,
//    ARDUCAM_SETTING_SHARPNESS,
//    ARDUCAM_SETTING_HUE,
//    ARDUCAM_SETTING_ZOOM,
//    ARDUCAM_SETTING_AUTOEXPOSURE,
//    ARDUCAM_SETTING_WHITEBALANCE,
//    ARDUCAM_SETTING_SPECIALEFFECT,
//    ARDUCAM_SETTING_LIGHTFREQUENCY,
    //ARDUCAM_SETTING_FRAMERATE,
    ARDUCAM_SETTING_MAX
} arducam_setting_type_t;

typedef enum
{
    ARDUCAM_TYPE_OV2640,
    ARDUCAM_TYPE_MAX
} arducam_type_t;

typedef enum
{
    ARDUCAM_RES_160x120,
    ARDUCAM_RES_176x144,
    ARDUCAM_RES_320x240,
    ARDUCAM_RES_352x288,
    ARDUCAM_RES_640x480,
    ARDUCAM_RES_800x600,
    ARDUCAM_RES_1024x768,
    ARDUCAM_RES_1280x1024,
    ARDUCAM_RES_1600x1200,
    ARDUCAM_RES_MAX
} arducam_resolution_t;



typedef enum
{
    ARDUCAM_FORMAT_JPEG,
    ARDUCAM_FORMAT_BMP,
} arducam_format_t;



typedef zos_result_t (*arducam_data_writer_t)(const void *image_chunk, uint32_t length, zos_bool_t last_chunk);
typedef void (*arducam_error_handler_t)(zos_result_t result);
typedef zos_bool_t (*arducam_image_ready_handler_t)(uint32_t image_size);


typedef struct
{
    arducam_resolution_t resolution;
    arducam_format_t format;
    uint8_t jpeg_quality;
} arducam_driver_config_t;

typedef struct
{
    arducam_data_writer_t data_writer;
    arducam_image_ready_handler_t image_ready;
    arducam_error_handler_t error_handler;
} arducam_callbacks_t;


typedef struct
{
    struct
    {
        zos_spi_t port;
        zos_gpio_t cs;
    } spi;
    zos_i2c_t i2c_port;

    uint16_t max_read_length;

    arducam_driver_config_t driver_config;
    arducam_callbacks_t callback;

} arducam_config_t;
