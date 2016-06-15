/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "hmc5883l.h"
#include "zos.h"
#include "sensor/types/magnetometer/magnetometer.h"

static void hmc5883l_data_to_standard(HMC5883L_MagnAxesRaw_t *in_data, magnetometer_data_t *out_data);

/*************************************************************************************************/
zos_result_t sensor_magnetometer_init(const magnetometer_config_t *config)
{
    if (HMC5883L_MagnInit() != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t sensor_magnetometer_has_new_data(zos_bool_t *has_data)
{
    zos_result_t result = ZOS_ERROR;
    uint8_t status_reg = 0;

    if (HMC5883L_MagnGetStatusReg(&status_reg) == ZOS_SUCCESS)
    {
        if ((status_reg & HMC5883L_DATA_RDY) > 0)
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
static void hmc5883l_data_to_standard(HMC5883L_MagnAxesRaw_t *in_data, magnetometer_data_t *out_data)
{
	out_data->x = (int32_t)in_data->HMC5883L_MAGN_AXIS_X;
	out_data->y = (int32_t)in_data->HMC5883L_MAGN_AXIS_Y;
	out_data->z = (int32_t)in_data->HMC5883L_MAGN_AXIS_Z;
}

/*************************************************************************************************/
/* Get latest data from accelerometer sensor */
zos_result_t sensor_magnetometer_get_data(magnetometer_data_t *data)
{
    zos_result_t result = ZOS_ERROR;
    HMC5883L_MagnAxesRaw_t raw_data = { 0 };
    if(HMC5883L_MagnGetAxesRaw(&raw_data) == ZOS_SUCCESS)
    {
        hmc5883l_data_to_standard(&raw_data, data);
        result = ZOS_SUCCESS;
    }
    return result;
}
