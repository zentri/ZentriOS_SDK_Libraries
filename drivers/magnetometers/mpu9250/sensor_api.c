/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "zos.h"
#include "sensor/types/magnetometer/magnetometer.h"
#include "mpu9250.h"



MPU9250_AK8963_MagnSensAdjust_t sensor_magnetometer_adjust_data = { 0 };



static void mpu9250_calculate_ut_values(MPU9250_AK8963_MagnAxesRaw_t *raw, magnetometer_data_t *output);




/*************************************************************************************************/
zos_result_t sensor_magnetometer_init(const magnetometer_config_t *config)
{
    if (MPU9250_MagnSetI2CBypass(MPU9250_BYPASS_ENABLED) != ZOS_SUCCESS)
        return ZOS_ERROR;

    // Read sensor sensitivity adjustment values
    if(MPU9250_AK8963_MagnReadSensAdj(&sensor_magnetometer_adjust_data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    if(MPU9250_AK8963_MagnSetOutBitSize(MPU9250_AK8963_MAGN_OUTPUT_14) != ZOS_SUCCESS)
        return ZOS_ERROR;

    // Request first magnetometer single measurement
    if(MPU9250_AK8963_MagnSetOprMode(MPU9250_AK8963_SNGL_MEASURE) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t sensor_magnetometer_has_new_data(zos_bool_t *has_data)
{
    zos_result_t result = ZOS_ERROR;

    uint8_t status_reg = 0;
    if (MPU9250_AK8963_MagnGetStatusReg(&status_reg) == ZOS_SUCCESS)
    {
        if ((status_reg & MPU9250_AK8963_DATA_RDY) > 0)
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
zos_result_t sensor_magnetometer_get_data(magnetometer_data_t *data)
{
    zos_result_t result = ZOS_ERROR;
    MPU9250_AK8963_MagnAxesRaw_t raw_data = { 0 };
    if (MPU9250_AK8963_MagnGetAxesRaw(&raw_data) == ZOS_SUCCESS)
    {
        mpu9250_calculate_ut_values(&raw_data, data);
        result = ZOS_SUCCESS;
    }

    /// Request next magnetometer single measurement
    MPU9250_AK8963_MagnSetOprMode(MPU9250_AK8963_SNGL_MEASURE);

    return result;
}



/*************************************************************************************************/
static void mpu9250_calculate_ut_values(MPU9250_AK8963_MagnAxesRaw_t *raw, magnetometer_data_t *output)
{
   // readings are in 1ut sensitivity shifted based on the output bit size:
   MPU9250_AK8963_MagnOutBitSize_t bit_size;
   if( MPU9250_AK8963_MagnGetOutBitSize(&bit_size) != ZOS_SUCCESS)
      output = MPU9250_AK8963_MAGN_OUTPUT_14;

   // Hadj = H * ((((ASA - 128) * 0.5) / 128) + 1)
   //      = H * (((ASA - 128) / 256 ) + 1)
   //      = H * ((ASA + 128) / 256)
   //      = (H * (ASA + 128)) >> 8;
   output->x = (raw->MPU9250_AK8963_MAGN_AXIS_X * (sensor_magnetometer_adjust_data.MPU9250_AK8963_MAGN_ADJUST_X + 128)) >> 8;
   output->y = (raw->MPU9250_AK8963_MAGN_AXIS_Y * (sensor_magnetometer_adjust_data.MPU9250_AK8963_MAGN_ADJUST_Y + 128)) >> 8;
   output->z = (raw->MPU9250_AK8963_MAGN_AXIS_Z * (sensor_magnetometer_adjust_data.MPU9250_AK8963_MAGN_ADJUST_Z + 128)) >> 8;

   switch(bit_size)
   {
       default:
       // phy_val = full_scale_range * reg_val / max_reg_val
       //         = 4912 * _MAGN_AXIS_ / (2^bit_size / 2)
       //         = 4912 * _MAGN_AXIS_ / 8192
       //         ~ 6 * _MAGN_AXIS_ / 10
       // (i.e., 8192/4912 = 1.668 LSB/uT: or 0.6 uT/LSB)
       case MPU9250_AK8963_MAGN_OUTPUT_14:
           output->x = (int32_t)(6 * output->x) / 10;
           output->y = (int32_t)(6 * output->y) / 10;
           output->z = (int32_t)(6 * output->z) / 10;
           break;

       // phy_val = full_scale_range * reg_val / max_reg_val
       //         = 4912 * _MAGN_AXIS_ / (2^bit_size / 2)
       //         = 4912 * _MAGN_AXIS_ / 32768
       //         ~ 15 * _MAGN_AXIS_ / 100
       // (i.e., 32768/4912 = 6.67 LSB/uT: or 0.15 uT/LSB)
       case MPU9250_AK8963_MAGN_OUTPUT_16:
           output->x = (int32_t)(15 * output->x) / 100;
           output->y = (int32_t)(15 * output->y) / 100;
           output->z = (int32_t)(15 * output->z) / 100;
           break;
   }
}
