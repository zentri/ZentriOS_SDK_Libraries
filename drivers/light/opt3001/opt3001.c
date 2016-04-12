/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "opt3001.h"


#define LIGHT_SENSOR_I2C_SLAVE_ADD  0x44
#define LIGHT_CONFIG_REG            0x01 // light sensor configuration register
#define RESULT_REG                  0x00 // light sensor status/result register

static const zos_i2c_device_t i2c_opt3001 =
{
    .port    = PLATFORM_STD_I2C,
    .address = LIGHT_SENSOR_I2C_SLAVE_ADD,
    .speed   = I2C_CLOCK_STANDARD_SPEED,
    .retries = (uint16_t)3U,
    .read_timeout = 100,
    .flags   = 0
};

/*************************************************************************************************/
zos_result_t opt3001_init()
{
    uint8_t light_sensor_config_data[2] = {0xCC, 0x10};
    return zn_i2c_master_write_reg(&i2c_opt3001, LIGHT_CONFIG_REG, light_sensor_config_data, 2);
}

/*************************************************************************************************/
zos_result_t opt3001_light_read(uint16_t *raw_lux)
{
	uint16_t buffer;
	zos_result_t result = zn_i2c_master_read_reg(&i2c_opt3001, RESULT_REG, (uint8_t*)&buffer, 2);

	buffer = htons(buffer);
	*raw_lux = buffer;
	return result;
}

