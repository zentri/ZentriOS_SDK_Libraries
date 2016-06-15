/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "lsm6ds0.h"
#include "zos.h"
#include "sensor/types/accelerometer/accelerometer.h"


static LSM6DS0_ODR_t get_odr(accel_sample_freq_t samp_freq);
static LSM6DS0_Fullscale_t get_fullscale(accel_fullscale_t scale);
static LSM6DS0_AXISenable_t get_axis_en(accel_axis_en_t axis_en);
static void calculate_g_values(AxesRaw_t *raw, accelerometer_data_t *output);

static LSM6DS0_Fullscale_t fullscale = LSM6DS0_FULLSCALE_2;

/*************************************************************************************************/
zos_result_t sensor_accelerometer_init(const accelerometer_config_t *config)
{
    zos_result_t result = ZOS_ERROR;


    if ((LSM6DS0_ResetDevice() == ZOS_SUCCESS) &&
        (LSM6DS0_SetODR(get_odr(config->samp_freq)) == ZOS_SUCCESS) &&
        (LSM6DS0_SetFullScale(get_fullscale(config->fullscale)) == ZOS_SUCCESS) &&
        (LSM6DS0_SetBLE(LSM6DS0_BLE_LSB) == ZOS_SUCCESS) &&
        (LSM6DS0_SetAxis(get_axis_en(config->axis_en)) == ZOS_SUCCESS))
    {
        result = LSM6DS0_Init();
    }

    return result;
}

/*************************************************************************************************/
zos_result_t sensor_accelerometer_has_new_data(zos_bool_t *has_data)
{
    zos_result_t result = ZOS_ERROR;
    uint8_t status_reg;

    if (LSM6DS0_GetStatusReg(&status_reg) == ZOS_SUCCESS)
    {
        if ((status_reg & LSM6DS0_I2C_ACCT_DATA_READY_BIT) > 0)
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
    AxesRaw_t raw_data = { 0 };
    if (LSM6DS0_GetAccAxesRaw(&raw_data) == ZOS_SUCCESS)
    {
        calculate_g_values(&raw_data, data);
        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
static LSM6DS0_ODR_t get_odr(accel_sample_freq_t samp_freq)
{
    LSM6DS0_ODR_t odr;

    switch (samp_freq)
    {
        case ACCEL_SAMP_FREQ_10HZ:
            odr = LSM6DS0_G_ODR_14_9HZ;
            break;

        case ACCEL_SAMP_FREQ_50HZ:
            odr = LSM6DS0_G_ODR_59_5HZ;
            break;

        case ACCEL_SAMP_FREQ_100HZ:
            odr = LSM6DS0_G_ODR_119HZ;
            break;

        default:
        case ACCEL_SAMP_FREQ_200HZ:
            odr = LSM6DS0_G_ODR_238HZ;
            break;

        case ACCEL_SAMP_FREQ_400HZ:
            odr = LSM6DS0_G_ODR_476HZ;
            break;
    }

    return odr;
}


/*************************************************************************************************/
static LSM6DS0_Fullscale_t get_fullscale(accel_fullscale_t scale)
{
    switch(scale)
    {
        default:
        case ACCEL_FULLSCALE_2G:
            fullscale = LSM6DS0_FULLSCALE_2;
            break;

        case ACCEL_FULLSCALE_4G:
            fullscale = LSM6DS0_FULLSCALE_4;
            break;

        case ACCEL_FULLSCALE_8G:
            fullscale = LSM6DS0_FULLSCALE_8;
            break;

        case ACCEL_FULLSCALE_16G:
            fullscale = LSM6DS0_FULLSCALE_16;
            break;
    }

    return fullscale;
}

/*************************************************************************************************/
static LSM6DS0_AXISenable_t get_axis_en(accel_axis_en_t axis_en)
{
    LSM6DS0_AXISenable_t enable = (LSM6DS0_AXISenable_t)0;

    if ((axis_en & ACCEL_AXIS_EN_X) > 0)
        enable |= LSM6DS0_X_ENABLE;

    if ((axis_en & ACCEL_AXIS_EN_Y) > 0)
        enable |= LSM6DS0_Y_ENABLE;

    if ((axis_en & ACCEL_AXIS_EN_Z) > 0)
        enable |= LSM6DS0_Z_ENABLE;

    return enable;
}

/*************************************************************************************************/
static void calculate_g_values(AxesRaw_t *raw, accelerometer_data_t *output)
{
    // readings are in 1mg sensitivity shifted based on the full-scale:
    switch(fullscale)
    {
        default:
        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 2 * _ACC_AXIS / (2^bit_size / 2)
        //         = 2 * _ACC_AXIS / 32768
        // (i.e., 32768/2 = 16,384 LSB/G = 0.061 mg/LSB)
        case LSM6DS0_FULLSCALE_2:
            output->x = (int32_t)(2 * 1000 * raw->AXIS_X) / 32768;
            output->y = (int32_t)(2 * 1000 * raw->AXIS_Y) / 32768;
            output->z = (int32_t)(2 * 1000 * raw->AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 4 * _ACC_AXIS / (2^bit_size / 2)
        //         = 4 * _ACC_AXIS / 32768
        // (i.e., 32768/4 = 8,192 LSB/G = 0.122 mg/LSB)
        case LSM6DS0_FULLSCALE_4:
            output->x = (int32_t)(4 * 1000 * raw->AXIS_X) / 32768;
            output->y = (int32_t)(4 * 1000 * raw->AXIS_Y) / 32768;
            output->z = (int32_t)(4 * 1000 * raw->AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 8 * _ACC_AXIS / (2^bit_size / 2)
        //         = 8 * _ACC_AXIS / 32768
        // (i.e., 32768/8 = 4,096 LSB/G = 0.244 mg/LSB)
        case LSM6DS0_FULLSCALE_8:
            output->x = (int32_t)(8 * 1000 * raw->AXIS_X) / 32768;
            output->y = (int32_t)(8 * 1000 * raw->AXIS_Y) / 32768;
            output->z = (int32_t)(8 * 1000 * raw->AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 16 * _ACC_AXIS / 21858
        // (i.e., 21858/16 = 1,366 LSB/G = 0.732 mg/LSB)
        // Note: 16g is handled differently as per datasheet
        case LSM6DS0_FULLSCALE_16:
            output->x = (int32_t)(16 * 1000 * raw->AXIS_X) / 21858;
            output->y = (int32_t)(16 * 1000 * raw->AXIS_Y) / 21858;
            output->z = (int32_t)(16 * 1000 * raw->AXIS_Z) / 21858;
            break;
    }
}
