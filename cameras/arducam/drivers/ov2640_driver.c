/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#ifndef ARDUCAM_OV2640_ENABLED

#define OV2640_DRIVER_CONFIG

#else

#define OV2640_SPI_RATE (8*1000*1000)
#define OV2640_SPI_FLAGS (SPI_FLAG_CLOCK_IDLE_LOW|SPI_FLAG_MSB_FIRST|SPI_FLAG_CLOCK_RISING_EDGE)

#define OV2640_I2C_ADDRESS 0x60
#define OV2640_I2C_CLOCK I2C_CLOCK_STANDARD_SPEED
#define OV2640_ADD_CALLBACK(name) .name = ov2640_ ## name


#define OV2640_DRIVER_CONFIG                    \
{                                               \
    .spi =                                      \
    {                                           \
        .rate = OV2640_SPI_RATE,                \
        .flags = OV2640_SPI_FLAGS               \
    },                                          \
    .i2c =                                      \
    {                                           \
        .rate OV2640_I2C_CLOCK,                 \
        .address = OV2640_I2C_ADDRESS,          \
        .retries = 3,                           \
        .read_timeout = 25,                     \
    },                                          \
    .callback =                                 \
    {                                           \
        OV2640_ADD_CALLBACK(init),              \
        OV2640_ADD_CALLBACK(start_capture),     \
        OV2640_ADD_CALLBACK(stop_capture),      \
        OV2640_ADD_CALLBACK(is_capture_ready),  \
        OV2640_ADD_CALLBACK(read_data),         \
        OV2640_ADD_CALLBACK(set_setting),       \
        OV2640_ADD_CALLBACK(get_setting)        \
    }                                           \
},



/*************************************************************************************************/
static zos_result_t ov2640_init(const arducam_settings_t *config)
{
    zos_result_t result;

    if(ZOS_FAILED(result, ov2640_validiate_id()))
    {
    }
    else if(ZOS_FAILED(result, ov2640_reset()))
    {
    }
    else if(ZOS_FAILED(result, ov2640_set_setting(ARDUCAM_SETTING_RESOLUTION, config->resolution)))
    {
    }
    else if(ZOS_FAILED(result, ov2640_set_setting(ARDUCAM_SETTING_FORMAT, config->format)))
    {
    }
    else if(ZOS_FAILED(result, ov2640_set_setting(ARDUCAM_SETTING_QUALITY, config->jpeg_quality)))
    {
    }

    return result;
}

/*************************************************************************************************/
static zos_result_t ov2640_validate_id(void)
{
    return ZOS_ERROR;
}

/*************************************************************************************************/
static zos_result_t ov2640_start_capture(void)
{
    return ZOS_ERROR;
}

/*************************************************************************************************/
static zos_result_t ov2640_stop_capture(void)
{
    return ZOS_ERROR;
}

/*************************************************************************************************/
static zos_result_t ov2640_is_capture_ready(uint32_t *image_size_ptr)
{
    return ZOS_ERROR;
}

/*************************************************************************************************/
static zos_result_t ov2640_read_data(void *data, uint16_t length)
{
    return ZOS_ERROR;
}

/*************************************************************************************************/
static zos_result_t ov2640_set_setting(arducam_setting_type_t setting, uint32_t value)
{
    return ZOS_ERROR;
}

/*************************************************************************************************/
static zos_result_t ov2640_get_setting(arducam_setting_type_t setting, uint32_t *value_ptr)
{
    return ZOS_ERROR;
}


#endif
