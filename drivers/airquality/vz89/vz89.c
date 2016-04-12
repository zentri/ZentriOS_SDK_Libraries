/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "vz89.h"



#define AIR_SENSOR_I2C_SLAVE_ADD    0x70
#define AIR_SET_PPMC02_REG          0x08 // calibrate air sensor command register
#define AIR_GET_STATUS_REG          0x09 // read air sensor status command register

static const zos_i2c_device_t i2c_vz89 =
{
    .port    = PLATFORM_STD_I2C,
    .address = AIR_SENSOR_I2C_SLAVE_ADD,
    .speed   = I2C_CLOCK_STANDARD_SPEED,
    .retries = (uint16_t)3U,
    .read_timeout = 100,
    .flags   = 0
};

/*************************************************************************************************/
zos_result_t vz89_airquality_read(sensor_data_t *raw_quality)
{
	zos_result_t result = zn_i2c_master_read_reg(&i2c_vz89, AIR_GET_STATUS_REG , (uint8_t*)raw_quality, sizeof(sensor_data_t));

	return result;
}

