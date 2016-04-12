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
#include "sensor/types/magnetometer/magnetometer.h"

static void fxos8700cq_calculate_ut_values(FXOS8700CQ_MagnAxesRaw_t *raw, magnetometer_data_t *output);

/*************************************************************************************************/
zos_result_t sensor_magnetometer_init(const magnetometer_config_t *config)
{
    // set to standby mode to set other configurations
    if (FXOS8700CQ_MagnSetOperatingMode(FXOS8700CQ_MAGN_MODE_STANDBY) != ZOS_SUCCESS)
        return ZOS_ERROR;

    // select hybrid mode with accelerometer and magnetometer active
    if (FXOS8700CQ_MagnSetHybridMode(FXOS8700CQ_MAGN_MODE_HYBRID) != ZOS_SUCCESS)
        return ZOS_ERROR;

    // Set auto calibration disabled, no one-shot magnetic reset, no one-shot magnetic measurement
    if (FXOS8700CQ_MagnSetControlParams(ZOS_FALSE, ZOS_FALSE, ZOS_FALSE) != ZOS_SUCCESS)
        return ZOS_ERROR;

    // set 8x over sampling (for 200Hz) to reduce magnetometer noise
    if (FXOS8700CQ_MagnSetOversampleRatio(FXOS8700CQ_MAGN_OSR_7) != ZOS_SUCCESS)
        return ZOS_ERROR;

    // set magnetometer auto increment
    if (FXOS8700CQ_MagnSetAutoIncrement(ZOS_TRUE) != ZOS_SUCCESS)
        return ZOS_ERROR;

    // exit standby to active operating mode
    if (FXOS8700CQ_MagnSetOperatingMode(FXOS8700CQ_MAGN_MODE_ACTIVE) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t sensor_magnetometer_has_new_data(zos_bool_t *has_data)
{
    zos_result_t result = ZOS_ERROR;

    uint8_t status_reg = 0;
    if (FXOS8700CQ_MagnGetStatusReg(&status_reg) == ZOS_SUCCESS)
    {
        if ((status_reg & FXOS8700CQ_DATA_READY) > 0)
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
    FXOS8700CQ_MagnAxesRaw_t raw_data = { 0 };
    if (FXOS8700CQ_MagnGetAxesRaw(&raw_data) == ZOS_SUCCESS)
    {
        fxos8700cq_calculate_ut_values(&raw_data, data);
        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
static void fxos8700cq_calculate_ut_values(FXOS8700CQ_MagnAxesRaw_t *raw, magnetometer_data_t *output)
{
    // The magnetometer sensitivity is fixed at 0.1 uT/LSB
    output->x = raw->FXOS8700CQ_MAGN_AXIS_X / 10;
    output->y = raw->FXOS8700CQ_MAGN_AXIS_Y / 10;
    output->z = raw->FXOS8700CQ_MAGN_AXIS_Z / 10;
}
