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

#include "drivers/arducam_drivers.h"
#include "ov2640.h"
#include "ov2640_regs.c"



#define OV2640_SPI_RATE (8*1000*1000)
#define OV2640_SPI_FLAGS (SPI_FLAG_CLOCK_IDLE_LOW|SPI_FLAG_MSB_FIRST|SPI_FLAG_CLOCK_RISING_EDGE)

#define OV2640_I2C_CLOCK I2C_CLOCK_STANDARD_SPEED


#define OV2640_WRITE_VERIFY(addr, val) if(adrucam_driver_i2c_write_reg(addr, val) != 0) return ZOS_ERROR



static uint32_t ov2640_settings[ARDUCAM_SETTING_MAX];


/*************************************************************************************************/
const adrucam_driver_t* ov2640_get_driver(void)
{
#define OV2640_ADD_CALLBACK(name) .name = ov2640_ ## name
    static const adrucam_driver_t driver =
    {
            .spi =
            {
                    .rate = OV2640_SPI_RATE,
                    .flags = OV2640_SPI_FLAGS
            },
            .i2c =
            {
                    .rate OV2640_I2C_CLOCK,
                    .address = OV2640_I2C_ADDRESS,
                    .retries = 3,
                    .read_timeout = 25,
            },
            .callback =
            {
                    OV2640_ADD_CALLBACK(init),
                    OV2640_ADD_CALLBACK(start_capture),
                    OV2640_ADD_CALLBACK(stop_capture),
                    OV2640_ADD_CALLBACK(is_capture_ready),
                    OV2640_ADD_CALLBACK(read_data),
                    OV2640_ADD_CALLBACK(set_setting),
                    OV2640_ADD_CALLBACK(get_setting)
            }
    };

    return &driver;
}



/*************************************************************************************************/
static zos_result_t ov2640_init(const arducam_driver_config_t *config)
{
    zos_result_t result;

    if(ZOS_FAILED(result, ov2640_validate_id()))
    {
    }
    else if(ZOS_FAILED(result, ov2640_reset()))
    {
    }
//    else if(ZOS_FAILED(result, ov2640_set_setting(ARDUCAM_SETTING_FORMAT, config->format)))
//    {
//    }
    else if(ZOS_FAILED(result, ov2640_set_setting(ARDUCAM_SETTING_RESOLUTION, config->resolution)))
    {
    }
    else if((config->format == ARDUCAM_FORMAT_JPEG) &&
            ZOS_FAILED(result, ov2640_set_setting(ARDUCAM_SETTING_QUALITY, config->jpeg_quality)))
    {
    }

    return result;
}

/*************************************************************************************************/
static zos_result_t ov2640_validate_id(void)
{
    uint8_t dummy;
    ov2640_id_t id;

    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_SENSOR);

    id.Manufacturer_ID1 = adrucam_driver_i2c_read_reg(MIDH, &dummy);
    id.Manufacturer_ID2 = adrucam_driver_i2c_read_reg(MIDL, &dummy);
    id.PIDH = adrucam_driver_i2c_read_reg(REG_PID, &dummy);
    id.PIDL = adrucam_driver_i2c_read_reg(REG_VER, &dummy);

    return (id.PIDH == OV2640_ID) ? ZOS_SUCCESS : ZOS_UNSUPPORTED;
}

/*************************************************************************************************/
static zos_result_t ov2640_reset(void)
{
    memset(ov2640_settings, 0, sizeof(ov2640_settings));

    ov2640_settings[ARDUCAM_SETTING_QUALITY] = QS_DEFAULT;
    ov2640_settings[ARDUCAM_SETTING_FORMAT] = ARDUCAM_FORMAT_JPEG;
    ov2640_settings[ARDUCAM_SETTING_RESOLUTION] = ARDUCAM_RES_320x240;
    ov2640_settings[ARDUCAM_SETTING_GAINCEILING] = COM9_DEFAULT;

    // put the sensor into reset
    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_SENSOR);
    OV2640_WRITE_VERIFY(COM7, COM7_SRST);
    zn_rtos_delay_milliseconds(100);

    ZOS_VERIFY(adrucam_driver_i2c_write_regs(ov2640_default_regs, NULL, 0));
    ZOS_VERIFY(adrucam_driver_i2c_write_regs(ov2640_yuv422_regs, NULL, 0));
    ZOS_VERIFY(adrucam_driver_i2c_write_regs(ov2640_jpeg_regs, NULL, 0));

    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_SENSOR);
    OV2640_WRITE_VERIFY(0x15, 0x00);
    //ZOS_VERIFY(adrucam_driver_i2c_write_regs(ov2640_320x240_jpeg, NULL, 0));
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
    zos_result_t result;

    switch(setting)
    {
    case ARDUCAM_SETTING_FORMAT:
        result = set_format(value);
        break;
    case ARDUCAM_SETTING_RESOLUTION:
        result = set_resolution(value);
        break;
    case ARDUCAM_SETTING_BRIGHTNESS:
        result = set_brightness(value);
        break;
    case ARDUCAM_SETTING_CONTRAST:
        result = set_contrast(value);
        break;
    case ARDUCAM_SETTING_SATURATION:
        result = set_saturation(value);
        break;
    case ARDUCAM_SETTING_GAINCEILING:
        result = set_gainceiling(value);
        break;
    case ARDUCAM_SETTING_QUALITY:
        result = set_quality(value);
        break;
    case ARDUCAM_SETTING_MIRROR:
        result = set_mirror(value);
        break;
    case ARDUCAM_SETTING_FLIP:
        result = set_flip(value);
        break;
//    case ARDUCAM_SETTING_HUE:
//        result = set_hue(value);
//        break;
//    case ARDUCAM_SETTING_SHARPNESS:
//        result = set_sharpness(value);
//        break;
//    case ARDUCAM_SETTING_AUTOEXPOSURE:
//        result = set_autoexposure(value);
//        break;
//    case ARDUCAM_SETTING_WHITEBALANCE:
//        result = set_whitebalance(value);
//        break;
//    case ARDUCAM_SETTING_SPECIALEFFECT:
//        result = set_specialeffect(value);
//        break;
//    case ARDUCAM_SETTING_FRAMERATE:
//        result = set_framerate(value);
//        break;
//    case ARDUCAM_SETTING_LIGHTFREQUENCY:
//        result = set_lightfrequency(value);
//        break;
//    case ARDUCAM_SETTING_ZOOM:
//        result = set_zoom(value);
//        break;
    default:
        result = ZOS_INVALID_ARG;
        break;
    }

    return result;
}

/*************************************************************************************************/
static zos_result_t ov2640_get_setting(arducam_setting_type_t setting, uint32_t *value_ptr)
{
    if(setting < ARDUCAM_SETTING_MAX)
    {
        return ov2640_settings[setting];
    }
    else
    {
        return UINT32_MAX;
    }
}



/*************************************************************************************************/
static zos_result_t set_contrast(int level )
{
    const int8_t val = (int8_t)(level + (ARDUCAM_CONTRAST_LEVELS / 2 + 1));
    if (val < 0 || val > ARDUCAM_CONTRAST_LEVELS)
    {
        return ZOS_INVALID_ARG;
    }

    /* Switch to DSP register bank */
    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_DSP);

    ZOS_VERIFY(adrucam_driver_i2c_write_reg_list(ov2640_contrast_regs[val], ARRAY_COUNT(ov2640_contrast_regs[0])));

    ov2640_settings[ARDUCAM_SETTING_CONTRAST] = level;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static zos_result_t set_saturation(  int level )
{
    const int8_t val = (int8_t)(level + (ARDUCAM_SATURATION_LEVELS / 2 + 1));
    if (val < 0 || val > ARDUCAM_SATURATION_LEVELS)
    {
        return ZOS_INVALID_ARG;
    }

    /* Switch to DSP register bank */
    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_DSP);

    /* Write contrast registers */
    ZOS_VERIFY(adrucam_driver_i2c_write_reg_list(ov2640_saturation_regs[val], ARRAY_COUNT(ov2640_saturation_regs[0])));

    ov2640_settings[ARDUCAM_SETTING_SATURATION] = level;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static zos_result_t set_brightness(  int level )
{
    const int8_t val = (int8_t)(level + (ARDUCAM_BRIGHTNESS_LEVELS / 2 + 1));
    if (val < 0 || val > ARDUCAM_BRIGHTNESS_LEVELS)
    {
        return ZOS_INVALID_ARG;
    }

    /* Switch to DSP register bank */
    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_DSP);

    /* Write contrast registers */
    ZOS_VERIFY(adrucam_driver_i2c_write_reg_list(ov2640_brightness_regs[val], ARRAY_COUNT(ov2640_brightness_regs[0])));

    ov2640_settings[ARDUCAM_SETTING_BRIGHTNESS] = level;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static zos_result_t set_format(int format)
{
    return ZOS_UNSUPPORTED;
}

/*************************************************************************************************/
static zos_result_t set_resolution(int resolution)
{
#define ADD_RES(res) case ARDUCAM_RES_ ## res: reg_list = ov2640_ ## res ## _jpeg; break;
    const reg_addr_value_t *reg_list;

    if(ov2640_settings[ARDUCAM_SETTING_FORMAT] != ARDUCAM_FORMAT_JPEG)
    {
        return ZOS_UNSUPPORTED;
    }
    switch(resolution)
    {
        ADD_RES(160x120)
        ADD_RES(176x144)
        ADD_RES(320x240)
        ADD_RES(352x288)
        ADD_RES(640x480)
        ADD_RES(800x600)
        ADD_RES(1024x768)
        ADD_RES(1280x1024)
        ADD_RES(1600x1200)
    default:
        return ZOS_UNSUPPORTED;
    }

    ZOS_VERIFY(adrucam_driver_i2c_write_regs(reg_list, NULL, 0));

    ov2640_settings[ARDUCAM_SETTING_RESOLUTION] = resolution;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static zos_result_t set_gainceiling(int gainceiling)
{
    if(gainceiling < ARDUCAM_GAINCEILING_MIN || gainceiling > ARDUCAM_GAINCEILING_MAX)
    {
        return ZOS_INVALID_ARG;
    }
    /* Switch to SENSOR register bank */
    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_SENSOR);

    /* Write gain ceiling register */
    OV2640_WRITE_VERIFY(COM9, COM9_AGC_SET(gainceiling));

    ov2640_settings[ARDUCAM_SETTING_GAINCEILING] = gainceiling;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static zos_result_t set_quality(int qs)
{
    if(qs < ARDUCAM_QUALITY_MIN || qs > ARDUCAM_QUALITY_MAX)
    {
        return ZOS_INVALID_ARG;
    }

    const int8_t val = -qs + QS_MAX;

    /* Switch to DSP register bank */
    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_DSP);

    /* Write QS register */
    OV2640_WRITE_VERIFY(QS, val);

    ov2640_settings[ARDUCAM_SETTING_QUALITY] = qs;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static zos_result_t set_flip(int flip)
{
    uint8_t reg04;
    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_SENSOR);
    ZOS_VERIFY(adrucam_driver_i2c_read_reg( 0x04, &reg04));
    if( flip == 0 )
    {
        reg04 &= 0xAF;
    }
    else
    {
        reg04 |= 0x50;
    }

    OV2640_WRITE_VERIFY(0x04,reg04);

    ov2640_settings[ARDUCAM_SETTING_FLIP] = flip;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static zos_result_t set_mirror(int mirror)
{
    uint8_t reg04;

    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_SENSOR);
    ZOS_VERIFY(adrucam_driver_i2c_read_reg( 0x04, &reg04));

    if( mirror == 0 )
    {
        reg04 &= 0x7F;
    }
    else
    {
        reg04 |= 0x80;
    }

    OV2640_WRITE_VERIFY(0x04,reg04);

    ov2640_settings[ARDUCAM_SETTING_MIRROR] = mirror;

    return ZOS_SUCCESS;
}



#endif
