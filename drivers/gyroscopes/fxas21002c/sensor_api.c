/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "fxas21002c.h"
#include "zos.h"
#include "sensor/types/gyroscope/gyroscope.h"



static FXAS21002C_GyroFullscale_t fxas21002c_get_fullscale(gyro_fullscale_t scale);
static void fxas21002c_calculate_dps_values(FXAS21002C_GyroAxesRaw_t *raw, gyroscope_data_t *output);

static FXAS21002C_GyroFullscale_t fullscale = FXAS21002C_GYRO_FULLSCALE_250;
/*************************************************************************************************/
zos_result_t sensor_gyroscope_init(const gyroscope_config_t *config)
{
    zos_result_t result = ZOS_ERROR;

    if ((FXAS21002C_GyroSetOperatingMode(FXAS21002C_MODE_STANDBY) == ZOS_SUCCESS) &&
        (FXAS21002C_GyroSetODR(FXAS21002C_ODR_200_0Hz) == ZOS_SUCCESS) &&   // hardcoded for now as not part of the config
        (FXAS21002C_GyroSetFullScale(fxas21002c_get_fullscale(config->fullscale)) == ZOS_SUCCESS) &&
        (FXAS21002C_GyroSetOperatingMode(FXAS21002C_MODE_ACTIVE) == ZOS_SUCCESS))
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
    if (FXAS21002C_GyroGetStatusReg(&status_reg) == ZOS_SUCCESS)
    {
        if ((status_reg & FXAS21002C_DATA_READY) > 0)
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
    FXAS21002C_GyroAxesRaw_t raw_data = { 0 };
    if (FXAS21002C_GyroGetAxesRaw(&raw_data) == ZOS_SUCCESS)
    {
        fxas21002c_calculate_dps_values(&raw_data, data);
        result = ZOS_SUCCESS;
    }

    return result;
}




/*************************************************************************************************/
static FXAS21002C_GyroFullscale_t fxas21002c_get_fullscale(gyro_fullscale_t scale)
{
    switch(scale)
    {
        default:
        case GYRO_FULLSCALE_250DPS:
            fullscale = FXAS21002C_GYRO_FULLSCALE_250;
            break;

        case GYRO_FULLSCALE_500DPS:
            fullscale = FXAS21002C_GYRO_FULLSCALE_500;
            break;

        case GYRO_FULLSCALE_1000DPS:
            fullscale = FXAS21002C_GYRO_FULLSCALE_1000;
            break;

        case GYRO_FULLSCALE_2000DPS:
            fullscale = FXAS21002C_GYRO_FULLSCALE_2000;
            break;
    }

    return fullscale;
}

static void fxas21002c_calculate_dps_values(FXAS21002C_GyroAxesRaw_t *raw, gyroscope_data_t *output)
{
    // readings are in 1dps sensitivity shifted based on the full-scale:
    switch(fullscale)
    {
        default:
        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 250 * _GYRO_AXIS_ / (2^bit_size / 2)
        //         = 250 * _GYRO_AXIS_ / 32768
        // (i.e., 32768/250 = 131 LSB/degree)
        case FXAS21002C_GYRO_FULLSCALE_250:
            output->x = (int32_t)(250 * raw->FXAS21002C_GYRO_AXIS_X) / 32768;
            output->y = (int32_t)(250 * raw->FXAS21002C_GYRO_AXIS_Y) / 32768;
            output->z = (int32_t)(250 * raw->FXAS21002C_GYRO_AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 500 * _GYRO_AXIS_ / (2^bit_size / 2)
        //         = 500 * _GYRO_AXIS_ / 32768
        // (i.e., 32768/500 = 65.5 LSB/degree)
        case FXAS21002C_GYRO_FULLSCALE_500:
            output->x = (int32_t)(500 * raw->FXAS21002C_GYRO_AXIS_X) / 32768;
            output->y = (int32_t)(500 * raw->FXAS21002C_GYRO_AXIS_Y) / 32768;
            output->z = (int32_t)(500 * raw->FXAS21002C_GYRO_AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 1000 * _GYRO_AXIS_ / (2^bit_size / 2)
        //         = 1000 * _GYRO_AXIS_ / 32768
        // (i.e., 32768/1000 = 32.8 LSB/degree)
        case FXAS21002C_GYRO_FULLSCALE_1000:
            output->x = (int32_t)(1000 * raw->FXAS21002C_GYRO_AXIS_X) / 32768;
            output->y = (int32_t)(1000 * raw->FXAS21002C_GYRO_AXIS_Y) / 32768;
            output->z = (int32_t)(1000 * raw->FXAS21002C_GYRO_AXIS_Z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 2000 * _GYRO_AXIS_ / (2^bit_size / 2)
        //         = 2000 * _GYRO_AXIS_ / 32768
        // (i.e., 32768/2000 = 16.4 LSB/degree)
        case FXAS21002C_GYRO_FULLSCALE_2000:
            output->x = (int32_t)(2000 * raw->FXAS21002C_GYRO_AXIS_X) / 32768;
            output->y = (int32_t)(2000 * raw->FXAS21002C_GYRO_AXIS_Y) / 32768;
            output->z = (int32_t)(2000 * raw->FXAS21002C_GYRO_AXIS_Z) / 32768;
            break;
    }
}
