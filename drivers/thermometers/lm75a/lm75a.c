/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "lm75a.h"


#define LM75A_SLAVE_ADDRESS 0x4C


#define LM75A_REG_TEMP 0x00


static const zos_i2c_device_t ic2_lm7a =
{
#ifdef PLATFORM_LM75A_I2C_PORT
	.port = PLATFORM_LM75A_I2C_PORT,
#endif
    .address = (uint16_t)LM75A_SLAVE_ADDRESS,
    .speed = I2C_CLOCK_STANDARD_SPEED,
    .retries = (uint16_t)3U,
    .read_timeout = 40
};



/*************************************************************************************************/
zos_result_t lm75a_temperature_read(uint16_t *raw_temp)
{
	uint16_t buffer;
	zos_result_t result = zn_i2c_master_read_reg(&ic2_lm7a, LM75A_REG_TEMP, (uint8_t*)&buffer, 2);
	buffer = htons(buffer);
	*raw_temp = (buffer >> 5);
	return result;
}

