/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "zos.h"
#include "sensor/types/gyroscope/gyroscope.h"
#include "mpu9250_gyro.h"



static MPU9250_GyroFullscale_t mpu9250_get_fullscale(gyro_fullscale_t scale);
static void mpu9250_calculate_dps_values(MPU9250_GyroAxesRaw_t *raw, gyroscope_data_t *output);


/*************************************************************************************************/
zos_result_t sensor_gyroscope_init(const gyroscope_config_t *config)
{
    zos_result_t result = ZOS_ERROR;

    if ((MPU9250_GyroSetPowerMgmt(MPU9250_RESET_DISABLED, MPU9250_SLEEP_DISABLED, MPU9250_CYCLE_DISABLED) == ZOS_SUCCESS) &&
        (MPU9250_GyroSetFullScale(mpu9250_get_fullscale(config->fullscale)) == ZOS_SUCCESS) &&
        (MPU9250_GyroSetAxis((config->axis_en & GYRO_AXIS_EN_X), (config->axis_en & GYRO_AXIS_EN_Y), (config->axis_en & GYRO_AXIS_EN_Z)) == ZOS_SUCCESS))
    {
        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
zos_result_t sensor_gyroscope_has_new_data(zos_bool_t *has_data)
{
    zos_result_t result = ZOS_ERROR;

    uint8_t status_reg = 0;
    if (MPU9250_GyroGetStatusReg(&status_reg) == ZOS_SUCCESS)
    {
        if ((status_reg & MPU9250_RAW_DATA_RDY_INT) > 0)
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
zos_result_t sensor_gyroscope_get_data(gyroscope_data_t *data)
{
    zos_result_t result = ZOS_ERROR;
    MPU9250_GyroAxesRaw_t raw_data = { 0 };
    if (MPU9250_GyroGetAxesRaw(&raw_data) == ZOS_SUCCESS)
    {
        mpu9250_calculate_dps_values(&raw_data, data);
        result = ZOS_SUCCESS;
    }

    return result;
}




/*************************************************************************************************/
static MPU9250_GyroFullscale_t mpu9250_get_fullscale(gyro_fullscale_t scale)
{
    MPU9250_GyroFullscale_t fullscale;

    switch(scale)
    {
        default:
        case GYRO_FULLSCALE_250DPS:
            fullscale = MPU9250_GYRO_FULLSCALE_250;
            break;

        case GYRO_FULLSCALE_500DPS:
            fullscale = MPU9250_GYRO_FULLSCALE_500;
            break;

        case GYRO_FULLSCALE_1000DPS:
            fullscale = MPU9250_GYRO_FULLSCALE_1000;
            break;

        case GYRO_FULLSCALE_2000DPS:
            fullscale = MPU9250_GYRO_FULLSCALE_2000;
            break;
    }

    return fullscale;
}

static void mpu9250_calculate_dps_values(MPU9250_GyroAxesRaw_t *raw, gyroscope_data_t *output)
{
    // readings are in 1dps sensitivity shifted based on the full-scale:
    MPU9250_GyroFullscale_t fs;
    if( MPU9250_GyroGetFullScale(&fs) != ZOS_SUCCESS)
        fs = MPU9250_GYRO_FULLSCALE_250;

    switch(fs)
    {
        default:
        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 250 * _GYRO_AXIS_ / (2^bit_size / 2)
        //         = 250 * _GYRO_AXIS_ / 32768
        // (i.e., 32768/250 = 131 LSB/degree)
        case MPU9250_GYRO_FULLSCALE_250:
            output->x = (int32_t)(250 * raw->MPU9250_GYRO_AXIS_X) / 32768;
            output->y = (int32_t)(250 * raw->MPU9250_GYRO_AXIS_Y) / 32768;
            output->z = (int32_t)(250 * raw->MPU9250_GYRO_AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 500 * _GYRO_AXIS_ / (2^bit_size / 2)
        //         = 500 * _GYRO_AXIS_ / 32768
        // (i.e., 32768/500 = 65.5 LSB/degree)
        case MPU9250_GYRO_FULLSCALE_500:
            output->x = (int32_t)(500 * raw->MPU9250_GYRO_AXIS_X) / 32768;
            output->y = (int32_t)(500 * raw->MPU9250_GYRO_AXIS_Y) / 32768;
            output->z = (int32_t)(500 * raw->MPU9250_GYRO_AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 1000 * _GYRO_AXIS_ / (2^bit_size / 2)
        //         = 1000 * _GYRO_AXIS_ / 32768
        // (i.e., 32768/1000 = 32.8 LSB/degree)
        case MPU9250_GYRO_FULLSCALE_1000:
            output->x = (int32_t)(1000 * raw->MPU9250_GYRO_AXIS_X) / 32768;
            output->y = (int32_t)(1000 * raw->MPU9250_GYRO_AXIS_Y) / 32768;
            output->z = (int32_t)(1000 * raw->MPU9250_GYRO_AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 2000 * _GYRO_AXIS_ / (2^bit_size / 2)
        //         = 2000 * _GYRO_AXIS_ / 32768
        // (i.e., 32768/2000 = 16.4 LSB/degree)
        case MPU9250_GYRO_FULLSCALE_2000:
            output->x = (int32_t)(2000 * raw->MPU9250_GYRO_AXIS_X) / 32768;
            output->y = (int32_t)(2000 * raw->MPU9250_GYRO_AXIS_Y) / 32768;
            output->z = (int32_t)(2000 * raw->MPU9250_GYRO_AXIS_Z) / 32768;
            break;
    }
}
