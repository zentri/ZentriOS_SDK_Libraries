/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "fxos8700cq.h"
#include "zos.h"
#include "sensor/types/accelerometer/accelerometer.h"


static FXOS8700CQ_ODR_t fxos8700cq_get_odr(accel_sample_freq_t samp_freq);
static FXOS8700CQ_AccFullscale_t fxos8700cq_get_fullscale(accel_fullscale_t scale);
static void fxos8700cq_calculate_g_values(FXOS8700CQ_AccAxesRaw_t *raw, accelerometer_data_t *output);

static FXOS8700CQ_AccFullscale_t fullscale = FXOS8700CQ_ACC_FULLSCALE_2;
/*************************************************************************************************/
zos_result_t sensor_accelerometer_init(const accelerometer_config_t *config)
{
    zos_result_t result = ZOS_ERROR;

    // for this sensor, fullscale should take values of 2, 4, or 8 only
    if(config->fullscale == ACCEL_FULLSCALE_16G)
    {
        return result;
    }

    if ((FXOS8700CQ_AccSetOperatingMode(FXOS8700CQ_MODE_STANDBY) == ZOS_SUCCESS) && // set to standby mode to set other configurations
        (FXOS8700CQ_AccSetHybridMode(FXOS8700CQ_MODE_HYBRID) == ZOS_SUCCESS) && // set hybrid mode active (for future use of magnetometer)
        (FXOS8700CQ_AccSetODR(fxos8700cq_get_odr(config->samp_freq)) == ZOS_SUCCESS) &&
        (FXOS8700CQ_AccSetFullScale(fxos8700cq_get_fullscale(config->fullscale)) == ZOS_SUCCESS) &&
        (FXOS8700CQ_AccSetOperatingMode(FXOS8700CQ_MODE_ACTIVE) == ZOS_SUCCESS)) // exit standby to active operating more
    {
        if((config->fullscale == ACCEL_FULLSCALE_2G) || (config->fullscale == ACCEL_FULLSCALE_4G))
        {
            // low noise mode is restricted only to 2g or 4g fs mode
            FXOS8700CQ_AccSetLowNoiseMode(FXOS8700CQ_LOW_NOISE_ACTV);
        }
        else
        {
            FXOS8700CQ_AccSetLowNoiseMode(FXOS8700CQ_LOW_NOISE_INACTV);
        }
        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
zos_result_t sensor_accelerometer_has_new_data(zos_bool_t *has_data)
{
    zos_result_t result = ZOS_ERROR;

    uint8_t status_reg = 0;
    if (FXOS8700CQ_AccGetStatusReg(&status_reg) == ZOS_SUCCESS)
    {
        if ((status_reg & 0xFE) > 0)
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
    FXOS8700CQ_AccAxesRaw_t raw_data = { 0 };
    if (FXOS8700CQ_AccGetAxesRaw(&raw_data) == ZOS_SUCCESS)
    {
        fxos8700cq_calculate_g_values(&raw_data, data);
        result = ZOS_SUCCESS;
    }

    return result;
}





/*************************************************************************************************/
static FXOS8700CQ_ODR_t fxos8700cq_get_odr(accel_sample_freq_t samp_freq)
{
    FXOS8700CQ_ODR_t odr;

    // There is no 1-to-1 mapping here, but we choose the nearest ODR values
    // Note: values here assume sensor works in hybrid mode
    switch (samp_freq)
    {
        case ACCEL_SAMP_FREQ_1HZ:
            odr = FXOS8700CQ_ODR_0_7813Hz;
            break;

        case ACCEL_SAMP_FREQ_10HZ:
            odr = FXOS8700CQ_ODR_6_25Hz;
            break;

        case ACCEL_SAMP_FREQ_25HZ:
            odr = FXOS8700CQ_ODR_25_0Hz;
            break;

        default:
        case ACCEL_SAMP_FREQ_50HZ:
            odr = FXOS8700CQ_ODR_50_0Hz;
            break;

        case ACCEL_SAMP_FREQ_100HZ:
            odr = FXOS8700CQ_ODR_100_0Hz;
            break;

        case ACCEL_SAMP_FREQ_200HZ:
            odr = FXOS8700CQ_ODR_200_0Hz;
            break;

        case ACCEL_SAMP_FREQ_400HZ:
            odr = FXOS8700CQ_ODR_400_0Hz;
            break;
    }

    return odr;
}

/*************************************************************************************************/
static FXOS8700CQ_AccFullscale_t fxos8700cq_get_fullscale(accel_fullscale_t scale)
{
    switch(scale)
    {
        default:
        case ACCEL_FULLSCALE_2G:
            fullscale = FXOS8700CQ_ACC_FULLSCALE_2;
            break;

        case ACCEL_FULLSCALE_4G:
            fullscale = FXOS8700CQ_ACC_FULLSCALE_4;
            break;

        case ACCEL_FULLSCALE_8G:
            fullscale = FXOS8700CQ_ACC_FULLSCALE_8;
            break;
    }

    return fullscale;
}

/*************************************************************************************************/
static void fxos8700cq_calculate_g_values(FXOS8700CQ_AccAxesRaw_t *raw, accelerometer_data_t *output)
{
    // readings are in 1mg sensitivity shifted based on the full-scale:

    switch(fullscale)
    {
        default:
        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 2 * _ACC_AXIS / (2^bit_size / 2)
        //         = 2 * _ACC_AXIS / 8192
        // (i.e., 8192/2 = 4096 LSB/G = ±0.244 mg/LSB)
        case FXOS8700CQ_ACC_FULLSCALE_2:
            output->x = (int32_t)(2 * 1000 * raw->FXOS8700CQ_ACC_AXIS_X) / 8192;
            output->y = (int32_t)(2 * 1000 * raw->FXOS8700CQ_ACC_AXIS_Y) / 8192;
            output->z = (int32_t)(2 * 1000 * raw->FXOS8700CQ_ACC_AXIS_Z) / 8192;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 4 * _ACC_AXIS / (2^bit_size / 2)
        //         = 4 * _ACC_AXIS / 8192
        // (i.e., 8192/4 = 2048 LSB/G = ±0.488 mg/LSB)
        case FXOS8700CQ_ACC_FULLSCALE_4:
            output->x = (int32_t)(4 * 1000 * raw->FXOS8700CQ_ACC_AXIS_X) / 8192;
            output->y = (int32_t)(4 * 1000 * raw->FXOS8700CQ_ACC_AXIS_Y) / 8192;
            output->z = (int32_t)(4 * 1000 * raw->FXOS8700CQ_ACC_AXIS_Z) / 8192;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 8 * _ACC_AXIS / (2^bit_size / 2)
        //         = 8 * _ACC_AXIS / 8192
        // (i.e., 8192/8 = 1024 LSB/G = ±0.976 mg/LSB)
        case FXOS8700CQ_ACC_FULLSCALE_8:
            output->x = (int32_t)(8 * 1000 * raw->FXOS8700CQ_ACC_AXIS_X) / 8192;
            output->y = (int32_t)(8 * 1000 * raw->FXOS8700CQ_ACC_AXIS_Y) / 8192;
            output->z = (int32_t)(8 * 1000 * raw->FXOS8700CQ_ACC_AXIS_Z) / 8192;
            break;
    }
}
