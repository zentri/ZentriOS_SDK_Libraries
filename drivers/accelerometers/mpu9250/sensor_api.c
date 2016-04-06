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
#include "mpu9250_accel.h"



static MPU9250_ODR_t mpu9250_get_odr(accel_sample_freq_t samp_freq);
static MPU9250_AccFullscale_t mpu9250_get_fullscale(accel_fullscale_t scale);
static void mpu9250_calculate_g_values(MPU9250_AccAxesRaw_t *raw, accelerometer_data_t *output);



/*************************************************************************************************/
zos_result_t sensor_accelerometer_init(const accelerometer_config_t *config)
{
    zos_result_t result = ZOS_ERROR;

    if ((MPU9250_AccSetPowerMgmt(MPU9250_SLEEP_RST, MPU9250_CYCLE_RST, MPU9250_STANDBY_RST) == ZOS_SUCCESS) &&
        (MPU9250_AccSetAccAxis((config->axis_en & ACCEL_AXIS_EN_X), (config->axis_en & ACCEL_AXIS_EN_Y), (config->axis_en & ACCEL_AXIS_EN_Z)) == ZOS_SUCCESS) &&
        (MPU9250_AccSetGyroAxis(ACCEL_AXIS_DISABLE, ACCEL_AXIS_DISABLE, ACCEL_AXIS_DISABLE) == ZOS_SUCCESS) &&
        (MPU9250_AccSetLowPassFilter(1, 1) == ZOS_SUCCESS) &&
        (MPU9250_AccSetInterrupts(MPU9250_WOM_EN) == ZOS_SUCCESS) &&
        (MPU9250_AccSetIntelligenceControl(1, 1) == ZOS_SUCCESS) &&
        (MPU9250_AccSetWomThr(config->wake_accel_threshold) == ZOS_SUCCESS) &&
        (MPU9250_AccSetODR(mpu9250_get_odr(config->samp_freq)) == ZOS_SUCCESS) &&
        (MPU9250_AccSetPowerMgmt(MPU9250_SLEEP_RST, MPU9250_CYCLE_SET, MPU9250_STANDBY_RST) == ZOS_SUCCESS) &&
        (MPU9250_AccSetFullScale(mpu9250_get_fullscale(config->fullscale)) == ZOS_SUCCESS))
    {
        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
zos_result_t sensor_accelerometer_has_new_data(zos_bool_t *has_data)
{
    zos_result_t result = ZOS_ERROR;

    uint8_t status_reg = 0;
    if (MPU9250_AccGetStatusReg(&status_reg) == ZOS_SUCCESS)
    {
        if ((status_reg & MPU9250_WOM_INT) > 0)
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
    MPU9250_AccAxesRaw_t raw_data = { 0 };
    if (MPU9250_AccGetAxesRaw(&raw_data) == ZOS_SUCCESS)
    {
        mpu9250_calculate_g_values(&raw_data, data);
        result = ZOS_SUCCESS;
    }

    return result;
}





/*************************************************************************************************/
static MPU9250_ODR_t mpu9250_get_odr(accel_sample_freq_t samp_freq)
{
    MPU9250_ODR_t odr;

    // There is no 1-to-1 mapping here, but we choose the nearest ODR values
    switch (samp_freq)
    {
        case ACCEL_SAMP_FREQ_1HZ:
            odr = MPU9250_ODR_0_98Hz;
            break;

        case ACCEL_SAMP_FREQ_10HZ:
            odr = MPU9250_ODR_15_63Hz;
            break;

        case ACCEL_SAMP_FREQ_25HZ:
            odr = MPU9250_ODR_31_25Hz;
            break;

        default:
        case ACCEL_SAMP_FREQ_50HZ:
            odr = MPU9250_ODR_62_50Hz;
            break;

        case ACCEL_SAMP_FREQ_100HZ:
            odr = MPU9250_ODR_125_0Hz;
            break;

        case ACCEL_SAMP_FREQ_200HZ:
            odr = MPU9250_ODR_250_0Hz;
            break;

        case ACCEL_SAMP_FREQ_400HZ:
            odr = MPU9250_ODR_500_0Hz;
            break;
    }

    return odr;
}

/*************************************************************************************************/
static MPU9250_AccFullscale_t mpu9250_get_fullscale(accel_fullscale_t scale)
{
    MPU9250_AccFullscale_t fullscale;

    switch(scale)
    {
        default:
        case ACCEL_FULLSCALE_2G:
            fullscale = MPU9250_ACC_FULLSCALE_2;
            break;

        case ACCEL_FULLSCALE_4G:
            fullscale = MPU9250_ACC_FULLSCALE_4;
            break;

        case ACCEL_FULLSCALE_8G:
            fullscale = MPU9250_ACC_FULLSCALE_8;
            break;

        case ACCEL_FULLSCALE_16G:
            fullscale = MPU9250_ACC_FULLSCALE_16;
            break;
    }

    return fullscale;
}

/*************************************************************************************************/
static void mpu9250_calculate_g_values(MPU9250_AccAxesRaw_t *raw, accelerometer_data_t *output)
{
    // readings are in 1mg sensitivity shifted based on the full-scale:
    MPU9250_AccFullscale_t fs;
    if( MPU9250_AccGetFullScale(&fs) != ZOS_SUCCESS)
        fs = MPU9250_ACC_FULLSCALE_2;

    switch(fs)
    {
        default:
        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 2 * _ACC_AXIS / (2^bit_size / 2)
        //         = 2 * _ACC_AXIS / 32768
        // (i.e., 32768/2 = 16,384 LSB/G)
        case MPU9250_ACC_FULLSCALE_2:
            output->x = (int32_t)(2 * 1000 * raw->MPU9250_ACC_AXIS_X) / 32768;
            output->y = (int32_t)(2 * 1000 * raw->MPU9250_ACC_AXIS_Y) / 32768;
            output->z = (int32_t)(2 * 1000 * raw->MPU9250_ACC_AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 4 * _ACC_AXIS / (2^bit_size / 2)
        //         = 4 * _ACC_AXIS / 32768
        // (i.e., 32768/4 = 8,192 LSB/G)
        case MPU9250_ACC_FULLSCALE_4:
            output->x = (int32_t)(4 * 1000 * raw->MPU9250_ACC_AXIS_X) / 32768;
            output->y = (int32_t)(4 * 1000 * raw->MPU9250_ACC_AXIS_Y) / 32768;
            output->z = (int32_t)(4 * 1000 * raw->MPU9250_ACC_AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 8 * _ACC_AXIS / (2^bit_size / 2)
        //         = 8 * _ACC_AXIS / 32768
        // (i.e., 32768/8 = 4,096 LSB/G)
        case MPU9250_ACC_FULLSCALE_8:
            output->x = (int32_t)(8 * 1000 * raw->MPU9250_ACC_AXIS_X) / 32768;
            output->y = (int32_t)(8 * 1000 * raw->MPU9250_ACC_AXIS_Y) / 32768;
            output->z = (int32_t)(8 * 1000 * raw->MPU9250_ACC_AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 16 * _ACC_AXIS / (2^bit_size / 2)
        //         = 16 * _ACC_AXIS / 32768
        // (i.e., 32768/16 = 2,048 LSB/G)
        case MPU9250_ACC_FULLSCALE_16:
            output->x = (int32_t)(16 * 1000 * raw->MPU9250_ACC_AXIS_X) / 32768;
            output->y = (int32_t)(16 * 1000 * raw->MPU9250_ACC_AXIS_Y) / 32768;
            output->z = (int32_t)(16 * 1000 * raw->MPU9250_ACC_AXIS_Z) / 32768;
            break;
    }
}
