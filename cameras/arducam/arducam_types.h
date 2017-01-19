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

#define ARDUCAM_OV2640_ENABLED

typedef enum
{
    ARDUCAM_SETTING_RESOLUTION,
    ARDUCAM_SETTING_FORMAT,
    ARDUCAM_SETTING_BRIGHTNESS,
    ARDUCAM_SETTING_CONTRAST,
    ARDUCAM_SETTING_SATURATION,
    ARDUCAM_SETTING_GAINCEILING,
    ARDUCAM_SETTING_QUALITY,
    ARDUCAM_SETTING_MIRROR,
    ARDUCAM_SETTING_FLIP,
//    CAMERA_SETTING_SHARPNESS,
//    CAMERA_SETTING_HUE,
//    CAMERA_SETTING_ZOOM,
//    CAMERA_SETTING_AUTOEXPOSURE,
//    CAMERA_SETTING_WHITEBALANCE,
//    CAMERA_SETTING_SPECIALEFFECT,
//    CAMERA_SETTING_LIGHTFREQUENCY,
    //CAMERA_SETTING_FRAMERATE,
    ARDUCAM_SETTING_MAX
} arducam_setting_type_t;

typedef enum
{
#ifdef ARDUCAM_OV2640_ENABLED
    ARDUCAM_TYPE_OV2640,
#endif
    ARDUCAM_TYPE_MAX
} arducam_type_t;

typedef enum
{
    //CAMERA_RES_QVGA,       // QVGA:320*240
   // CAMERA_RES_VGA,        // VGA:640*480
    ARDUCAM_RES_SVGA,       // SVGA:800*600
   // CAMERA_RES_XGA,        // XGA:1024*768
    //CAMERA_RES_SXGA,       // SXGA:1280*1024
    ARDUCAM_RES_UXGA,       // SVGA:1600x1200
    ARDUCAM_RES_MAX = ARDUCAM_RES_UXGA+1
} arducam_resolution_t;



typedef enum
{
    ARDUCAM_FORMAT_RGB565,
    ARDUCAM_FORMAT_YUV422,
    ARDUCAM_FORMAT_GRAYSCALE,
    ARDUCAM_FORMAT_JPEG
} arducam_format_t;



typedef zos_result_t (*arducam_data_writer_t)(const void *image_chunk, uint32_t length, zos_bool_t last_chunk);
typedef zos_result_t (*arducam_error_handler_t)(zos_result_t result);
typedef zos_bool_t (*arducam_image_ready_handler_t)(uint32_t image_size);


typedef struct
{
    arducam_resolution_t resolution;
    arducam_format_t format;
    uint8_t jpeg_quality;
} arducam_settings_t;

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

    arducam_settings_t settings;
    arducam_callbacks_t callback;

} arducam_config_t;
