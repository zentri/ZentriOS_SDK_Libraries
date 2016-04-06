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
#include "bmi160.h"


static void bmi160_calculate_g_values(struct bmi160_accel_t *raw, accelerometer_data_t *output);



/*************************************************************************************************/
zos_result_t sensor_accelerometer_init(const accelerometer_config_t *config)
{
    zos_result_t result = ZOS_ERROR;

    const uint8_t rate = (config->samp_freq ==ACCEL_SAMP_FREQ_1HZ )   ? BMI160_ACCEL_OUTPUT_DATA_RATE_1_56HZ :
                         (config->samp_freq ==ACCEL_SAMP_FREQ_10HZ )  ? BMI160_ACCEL_OUTPUT_DATA_RATE_12_5HZ :
                         (config->samp_freq ==ACCEL_SAMP_FREQ_25HZ )  ? BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ :
                         (config->samp_freq ==ACCEL_SAMP_FREQ_50HZ )  ? BMI160_ACCEL_OUTPUT_DATA_RATE_50HZ :
                         (config->samp_freq ==ACCEL_SAMP_FREQ_100HZ ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ :
                         (config->samp_freq ==ACCEL_SAMP_FREQ_200HZ ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_200HZ : BMI160_ACCEL_OUTPUT_DATA_RATE_400HZ;
    const uint8_t range = (config->fullscale == ACCEL_FULLSCALE_2G) ? BMI160_ACCEL_RANGE_2G :
                          (config->fullscale == ACCEL_FULLSCALE_4G) ? BMI160_ACCEL_RANGE_4G :
                          (config->fullscale == ACCEL_FULLSCALE_8G) ? BMI160_ACCEL_RANGE_8G : BMI160_ACCEL_RANGE_16G;
    if(ZOS_FAILED(result, bmi160_init(NULL)))
    {
    }
    else if(ZOS_FAILED(result, bmi160_set_command_register(ACCEL_MODE_NORMAL)))
    {
    }
    else if(ZOS_FAILED(result, bmi160_set_accel_bw(BMI160_ACCEL_NORMAL_AVG4)))
    {
    }
    else if(ZOS_FAILED(result, bmi160_set_accel_output_data_rate(rate)))
    {
    }
    else if(ZOS_FAILED(result, bmi160_set_accel_range(range)))
    {
    }

    return result;
}

/*************************************************************************************************/
zos_result_t sensor_accelerometer_has_new_data(zos_bool_t *has_data)
{
    zos_result_t result;

    if(!ZOS_FAILED(result, bmi160_get_accel_data_rdy((uint8_t*)has_data)))
    {
    }

    return result;
}

/*************************************************************************************************/
/* Get latest data from accelerometer sensor */
zos_result_t sensor_accelerometer_get_data(accelerometer_data_t *data)
{
    zos_result_t result;
    struct bmi160_accel_t raw_data;
    if(!ZOS_FAILED(result, bmi160_read_accel_xyz(&raw_data)))
    {
        bmi160_calculate_g_values(&raw_data, data);
    }

    return result;
}



/*************************************************************************************************/
static void bmi160_calculate_g_values(struct bmi160_accel_t *raw, accelerometer_data_t *output)
{
    // readings are in 1mg sensitivity shifted based on the full-scale:
    uint8_t range;
    if( bmi160_get_accel_range(&range) != ZOS_SUCCESS)
        range = BMI160_ACCEL_RANGE_2G;

    switch(range)
    {
        default:
        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 2 * _ACC_AXIS / (2^bit_size / 2)
        //         = 2 * _ACC_AXIS / 32768
        // (i.e., 32768/2 = 16,384 LSB/G)
        case BMI160_ACCEL_RANGE_2G:
            output->x = (int32_t)(2 * 1000 * raw->x) / 32768;
            output->y = (int32_t)(2 * 1000 * raw->y) / 32768;
            output->z = (int32_t)(2 * 1000 * raw->z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 4 * _ACC_AXIS / (2^bit_size / 2)
        //         = 4 * _ACC_AXIS / 32768
        // (i.e., 32768/4 = 8,192 LSB/G)
        case BMI160_ACCEL_RANGE_4G:
            output->x = (int32_t)(4 * 1000 * raw->x) / 32768;
            output->y = (int32_t)(4 * 1000 * raw->y) / 32768;
            output->z = (int32_t)(4 * 1000 * raw->z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 8 * _ACC_AXIS / (2^bit_size / 2)
        //         = 8 * _ACC_AXIS / 32768
        // (i.e., 32768/8 = 4,096 LSB/G)
        case BMI160_ACCEL_RANGE_8G:
            output->x = (int32_t)(8 * 1000 * raw->x) / 32768;
            output->y = (int32_t)(8 * 1000 * raw->y) / 32768;
            output->z = (int32_t)(8 * 1000 * raw->z) / 32768;
            break;

        // phy_val = full_scale_range * reg_val / max_reg_val
        //         = 16 * _ACC_AXIS / (2^bit_size / 2)
        //         = 16 * _ACC_AXIS / 32768
        // (i.e., 32768/16 = 2,048 LSB/G)
        case BMI160_ACCEL_RANGE_16G:
            output->x = (int32_t)(16 * 1000 * raw->x) / 32768;
            output->y = (int32_t)(16 * 1000 * raw->y) / 32768;
            output->z = (int32_t)(16 * 1000 * raw->z) / 32768;
            break;
    }
}
