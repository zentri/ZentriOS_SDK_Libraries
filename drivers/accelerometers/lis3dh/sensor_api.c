/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "zos.h"
#include "sensor/types/accelerometer/accelerometer.h"
#include "lis3dh.h"


static LIS3DH_ODR_t get_odr(accel_sample_freq_t samp_freq);
static LIS3DH_Fullscale_t get_fullscale(accel_fullscale_t scale);
static LIS3DH_AXISenable_t get_axis_en(accel_axis_en_t axis_en);
static LIS3DH_Mode_t get_mode(accel_mode_t accel_mode);
static void calculate_g_values(AxesRaw_t *raw, accelerometer_data_t *output);



/*************************************************************************************************/
zos_result_t sensor_accelerometer_init(const accelerometer_config_t *config)
{
    zos_result_t result = ZOS_ERROR;

    if ((LIS3DH_SetODR(get_odr(config->samp_freq)) == MEMS_SUCCESS) &&
        (LIS3DH_SetMode(get_mode(config->mode)) == MEMS_SUCCESS) &&
        (LIS3DH_SetFullScale(get_fullscale(config->fullscale)) == MEMS_SUCCESS) &&
        (LIS3DH_SetBLE(LIS3DH_BLE_LSB) == MEMS_SUCCESS) &&
        (LIS3DH_SetAxis(get_axis_en(config->axis_en)) == MEMS_SUCCESS) &&
        (LIS3DH_SetInt6D4DConfiguration(LIS3DH_INT1_6D_4D_DISABLE) == MEMS_SUCCESS) &&
        (LIS3DH_SetIntMode(LIS3DH_INT_MODE_OR) == MEMS_SUCCESS))
    {
        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
zos_result_t sensor_accelerometer_has_new_data(zos_bool_t *has_data)
{
    zos_result_t result = ZOS_ERROR;

    u8_t status_reg;
    if (LIS3DH_GetStatusReg(&status_reg) == MEMS_SUCCESS)
    {
        if ((status_reg & LIS3DH_DATAREADY_BIT) > 0)
        {
            *has_data = ZOS_TRUE;
        }
        else
        {
            *has_data = ZOS_FALSE;
        }

        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
/* Get latest data from accelerometer sensor */
zos_result_t sensor_accelerometer_get_data(accelerometer_data_t *data)
{
    zos_result_t result = ZOS_ERROR;
    AxesRaw_t raw_data;
    if (LIS3DH_GetAccAxesRaw(&raw_data) == MEMS_SUCCESS)
    {
        calculate_g_values(&raw_data, data);
        result = ZOS_SUCCESS;
    }

    return result;
}




/*************************************************************************************************/
static LIS3DH_ODR_t get_odr(accel_sample_freq_t samp_freq)
{
    LIS3DH_ODR_t odr;

    switch (samp_freq)
    {
        case ACCEL_SAMP_FREQ_1HZ:
            odr = LIS3DH_ODR_1Hz;
            break;

        case ACCEL_SAMP_FREQ_10HZ:
            odr = LIS3DH_ODR_10Hz;
            break;

        case ACCEL_SAMP_FREQ_25HZ:
            odr = LIS3DH_ODR_25Hz;
            break;

        default:
        case ACCEL_SAMP_FREQ_50HZ:
            odr = LIS3DH_ODR_50Hz;
            break;

        case ACCEL_SAMP_FREQ_100HZ:
            odr = LIS3DH_ODR_100Hz;
            break;

        case ACCEL_SAMP_FREQ_200HZ:
            odr = LIS3DH_ODR_200Hz;
            break;

        case ACCEL_SAMP_FREQ_400HZ:
            odr = LIS3DH_ODR_400Hz;
            break;
    }

    return odr;
}


/*************************************************************************************************/
static LIS3DH_Fullscale_t get_fullscale(accel_fullscale_t scale)
{
    LIS3DH_Fullscale_t fullscale;

    switch(scale)
    {
        default:
        case ACCEL_FULLSCALE_2G:
            fullscale = LIS3DH_FULLSCALE_2;
            break;

        case ACCEL_FULLSCALE_4G:
            fullscale = LIS3DH_FULLSCALE_4;
            break;

        case ACCEL_FULLSCALE_8G:
            fullscale = LIS3DH_FULLSCALE_8;
            break;

        case ACCEL_FULLSCALE_16G:
            fullscale = LIS3DH_FULLSCALE_16;
            break;
    }

    return fullscale;
}

/*************************************************************************************************/
static LIS3DH_AXISenable_t get_axis_en(accel_axis_en_t axis_en)
{
    LIS3DH_AXISenable_t enable = (LIS3DH_AXISenable_t)0;

    if ((axis_en & ACCEL_AXIS_EN_X) > 0)
        enable |= LIS3DH_X_ENABLE;

    if ((axis_en & ACCEL_AXIS_EN_Y) > 0)
        enable |= LIS3DH_Y_ENABLE;

    if ((axis_en & ACCEL_AXIS_EN_Z) > 0)
        enable |= LIS3DH_Z_ENABLE;

    return enable;
}

/*************************************************************************************************/
static LIS3DH_Mode_t get_mode(accel_mode_t accel_mode)
{
    LIS3DH_Mode_t mode;

    switch(accel_mode)
    {
        default:
        case ACCEL_MODE_NORMAL:
            mode = LIS3DH_NORMAL;
            break;
        case ACCEL_MODE_LOW_POWER:
            mode = LIS3DH_LOW_POWER;
            break;
        case ACCEL_MODE_POWER_DOWN:
            mode = LIS3DH_POWER_DOWN;
            break;
    }

    return mode;
}

/*************************************************************************************************/
static void calculate_g_values(AxesRaw_t *raw, accelerometer_data_t *output)
{
    //for 2G fullscale, 1mg sensitivity, left justified 12-bit data
    output->x = (int32_t)(raw->AXIS_X / 16);
    output->y = (int32_t)(raw->AXIS_Y / 16);
    output->z = (int32_t)(raw->AXIS_Z / 16);
}
