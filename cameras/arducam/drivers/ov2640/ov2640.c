/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "drivers/arducam_drivers.h"
#include "drivers/arduchip.h"
#include "ov2640.h"
#include "ov2640_regs.c"


#define MAX_FIFO_SIZE       0x5FFFF         //384KByte


#define OV2640_SPI_RATE (8*1000*1000)
#define OV2640_SPI_FLAGS (SPI_FLAG_CLOCK_IDLE_LOW|SPI_FLAG_MSB_FIRST|SPI_FLAG_CLOCK_RISING_EDGE)

#define OV2640_I2C_CLOCK I2C_CLOCK_STANDARD_SPEED


#define OV2640_WRITE_VERIFY(addr, val) if(arducam_driver_i2c_write_reg(addr, val) != 0) return ZOS_ERROR



static uint32_t ov2640_settings[ARDUCAM_SETTING_MAX];


/*************************************************************************************************/
const arducam_driver_t* ov2640_get_driver(void)
{
#define OV2640_ADD_CALLBACK(name) .name = ov2640_ ## name
    static const arducam_driver_t driver =
    {
            .spi =
            {
                    .rate = OV2640_SPI_RATE,
                    .flags = OV2640_SPI_FLAGS
            },
            .i2c =
            {
                    .rate = OV2640_I2C_CLOCK,
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
                    OV2640_ADD_CALLBACK(reset),
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

    memset(ov2640_settings, 0, sizeof(ov2640_settings));

    ov2640_settings[ARDUCAM_SETTING_QUALITY] = config->jpeg_quality;
    ov2640_settings[ARDUCAM_SETTING_FORMAT] = config->format;
    ov2640_settings[ARDUCAM_SETTING_RESOLUTION] = config->resolution;
    ov2640_settings[ARDUCAM_SETTING_GAINCEILING] = COM9_DEFAULT;

    if(ZOS_FAILED(result, ov2640_validate()))
    {
    }
    else if(ZOS_FAILED(result, ov2640_reset()))
    {
    }

    return result;
}

/*************************************************************************************************/
static zos_result_t ov2640_validate(void)
{
#define SPI_TEST_VALUE 0xBA
    zos_result_t result;
    uint8_t reg_value;
    ov2640_id_t id;

    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_SENSOR);

    id.Manufacturer_ID1 = arducam_driver_i2c_read_reg(MIDH, &reg_value);
    id.Manufacturer_ID2 = arducam_driver_i2c_read_reg(MIDL, &reg_value);
    id.PIDH = arducam_driver_i2c_read_reg(REG_PID, &reg_value);
    id.PIDL = arducam_driver_i2c_read_reg(REG_VER, &reg_value);

    if(!(id.PIDH == OV2640_ID))
    {
        return ZOS_UNSUPPORTED;
    }

    reg_value = 0;
    arducam_driver_spi_read_reg(ARDUCHIP_REG_REV, &reg_value);
    ZOS_LOG("ARDUCHIP_REG_REV: 0x%02X", reg_value);

    reg_value = 0;
    arducam_driver_spi_write_reg(ARDUCHIP_REG_TEST1, SPI_TEST_VALUE);
    arducam_driver_spi_read_reg(ARDUCHIP_REG_TEST1, &reg_value);

    if(reg_value != SPI_TEST_VALUE)
    {
        ZOS_LOG("Failed to write SPI test register");
        return ZOS_UNINITIALIZED;
    }

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static zos_result_t ov2640_reset(void)
{
    // put the sensor into reset
    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_SENSOR);
    OV2640_WRITE_VERIFY(COM7, COM7_SRST);
    zn_rtos_delay_milliseconds(100);

    ZOS_VERIFY(arducam_driver_i2c_write_regs(ov2640_default_regs, NULL, 0));
    ZOS_VERIFY(arducam_driver_i2c_write_regs(ov2640_yuv422_regs, NULL, 0));
    ZOS_VERIFY(arducam_driver_i2c_write_regs(ov2640_jpeg_regs, NULL, 0));

    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_SENSOR);
    OV2640_WRITE_VERIFY(0x15, 0x00);
    //ZOS_VERIFY(ov2640_set_setting(ARDUCAM_SETTING_FORMAT, ov2640_settings[ARDUCAM_SETTING_FORMAT]));
    ZOS_VERIFY(ov2640_set_setting(ARDUCAM_SETTING_RESOLUTION, ov2640_settings[ARDUCAM_SETTING_RESOLUTION]));
    ZOS_VERIFY(ov2640_set_setting(ARDUCAM_SETTING_QUALITY, ov2640_settings[ARDUCAM_SETTING_QUALITY]));

    ZOS_VERIFY(arducam_driver_spi_clear_bit(ARDUCHIP_REG_FIFO, FIFO_CLEAR_MASK|FIFO_RDPTR_RST_MASK|FIFO_WRPTR_RST_MASK));
    ZOS_VERIFY(arducam_driver_spi_set_bit(ARDUCHIP_REG_TIM, MODE_MASK));
    ZOS_VERIFY(arducam_driver_spi_write_reg(ARDUCHIP_REG_FRAMES, 1));

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static zos_result_t ov2640_start_capture(void)
{
    return arducam_driver_spi_set_bit(ARDUCHIP_REG_FIFO, FIFO_CLEAR_MASK|FIFO_START_MASK);
}

/*************************************************************************************************/
static zos_result_t ov2640_stop_capture(void)
{
    return ZOS_UNIMPLEMENTED;
}

/*************************************************************************************************/
static zos_result_t ov2640_is_capture_ready(uint32_t *image_size_ptr)
{
    uint8_t status;

    ZOS_VERIFY(arducam_driver_spi_read_reg(ARDUCHIP_REG_STATUS, &status));

    if(status & CAP_DONE_MASK)
    {
        uint32_t size;
        uint8_t size_part;

        ZOS_VERIFY(arducam_driver_spi_read_reg(FIFO_SIZE3, &size_part));
        size = size_part;
        size <<= 8;

        ZOS_VERIFY(arducam_driver_spi_read_reg(FIFO_SIZE2, &size_part));
        size |= (uint32_t)size_part;
        size <<= 8;

        ZOS_VERIFY(arducam_driver_spi_read_reg(FIFO_SIZE1, &size_part));
        size |= (uint32_t)size_part;

        *image_size_ptr = size;

        return ZOS_SUCCESS;
    }
    else
    {
        return ZOS_PENDING;
    }
}

/*************************************************************************************************/
static zos_result_t ov2640_read_data(void *data, uint16_t length)
{
    return arducam_driver_spi_burst_read(data, length);
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
        *value_ptr = ov2640_settings[setting];
        return ZOS_SUCCESS;
    }
    else
    {
        return ZOS_INVALID_ARG;
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

    /* Write contrast registers */
    for (int i=0; i< ARRAY_COUNT(ov2640_contrast_regs[0]); i++)
    {
         OV2640_WRITE_VERIFY(ov2640_contrast_regs[0][i], ov2640_contrast_regs[val][i]);
    }

    ov2640_settings[ARDUCAM_SETTING_CONTRAST] = level;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static zos_result_t set_saturation(int level )
{
    const int8_t val = (int8_t)(level + (ARDUCAM_SATURATION_LEVELS / 2 + 1));
    if (val < 0 || val > ARDUCAM_SATURATION_LEVELS)
    {
        return ZOS_INVALID_ARG;
    }

    /* Switch to DSP register bank */
    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_DSP);

    /* Write contrast registers */
    for (int i=0; i< ARRAY_COUNT(ov2640_saturation_regs[0]); i++)
    {
         OV2640_WRITE_VERIFY(ov2640_saturation_regs[0][i], ov2640_saturation_regs[val][i]);
    }

    ov2640_settings[ARDUCAM_SETTING_SATURATION] = level;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static zos_result_t set_brightness(int level )
{
    const int8_t val = (int8_t)(level + (ARDUCAM_BRIGHTNESS_LEVELS / 2 + 1));
    if (val < 0 || val > ARDUCAM_BRIGHTNESS_LEVELS)
    {
        return ZOS_INVALID_ARG;
    }

    /* Switch to DSP register bank */
    OV2640_WRITE_VERIFY(BANK_SEL, BANK_SEL_DSP);

    /* Write contrast registers */
    for (int i=0; i< ARRAY_COUNT(ov2640_brightness_regs[0]); i++)
    {
         OV2640_WRITE_VERIFY(ov2640_brightness_regs[0][i], ov2640_brightness_regs[val][i]);
    }

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

    ZOS_VERIFY(arducam_driver_i2c_write_regs(reg_list, NULL, 0));

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
    ZOS_VERIFY(arducam_driver_i2c_read_reg( 0x04, &reg04));
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
    ZOS_VERIFY(arducam_driver_i2c_read_reg( 0x04, &reg04));

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
