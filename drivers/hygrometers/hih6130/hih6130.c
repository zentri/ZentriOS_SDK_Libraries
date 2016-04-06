/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "hih6130.h"


#define BITMASK_STATUS      0xC0
#define BITMASK_HUM_MSB     (uint8_t)~(BITMASK_STATUS)
#define BITMASK_TEMP_LSB    0xFC

#define OFFSET_STATUS       6

static const zos_i2c_device_t const ZOS_I2C_HIH6130 =
{
    .address = (uint16_t)HIH6130_SLAVE_ADDRESS,
    .speed = I2C_CLOCK_HIGH_SPEED,
    .retries = (uint16_t)1U,
    .flags = 0,
	.read_timeout = 40//ms
};


#define GET_STATUS(reg_val) ((hih6130_status_t)(reg_val & BITMASK_STATUS) >> OFFSET_STATUS)
#define GET_HUMIDITY(reg_msb, reg_lsb) (((uint16_t)(reg_msb & BITMASK_HUM_MSB) << 8) | ((uint16_t)reg_lsb))
#define GET_TEMPERATURE(reg_msb, reg_lsb) (((uint16_t)reg_msb << 6) | ((uint16_t)((reg_lsb & BITMASK_TEMP_LSB) >> 2)))



static zos_result_t read_data(uint8_t *result, uint16_t length);
static zos_result_t measure_and_read(uint8_t *data, uint16_t length);
static zos_result_t get_data(uint8_t* data, uint16_t length, zos_bool_t use_last);
static uint16_t get_temperature(const uint8_t* data);
static uint16_t get_humidity(const uint8_t* data);
static uint16_t get_status(const uint8_t* data);




/* Measure and read the relative humidity, temperature and status */
zos_result_t hih6130_read(hih6130_status_t* status, uint16_t* humidity, uint16_t* temperature, const zos_bool_t use_last)
{
    uint8_t data[4];
    zos_result_t result = get_data(data, sizeof(data), use_last);

    *status = get_status(data);
    *humidity = get_humidity(data);
    *temperature = get_temperature(data);

    return result;
}

/*************************************************************************************************/
/* Measure and read the relative humidity */
zos_result_t hih6130_humidity_read(uint16_t* humidity, const zos_bool_t use_last)
{
    uint8_t data[2];
    zos_result_t result = get_data(data, sizeof(data), use_last);

    *humidity = get_humidity(data);

    return result;
}

/*************************************************************************************************/
/* Read the ambient temperature (get last measurement or take new sample) */
zos_result_t hih6130_temperature_read(uint16_t *temperature, const zos_bool_t use_last)
{
    uint8_t data[4];
    zos_result_t result = get_data(data, sizeof(data), use_last);

    *temperature = get_temperature(data);

    return result;
}

/*************************************************************************************************/
/* Get status */
zos_result_t hih6130_status_get(hih6130_status_t* status)
{
    uint8_t data;
    zos_result_t result = get_data(&data, sizeof(data), ZOS_TRUE);

    *status = get_status(&data);

    return result;
}


/******************************************************
*          Internal Function Definitions
******************************************************/

/* Reads data from sensor without taking measurement */
static zos_result_t read_data(uint8_t *result, uint16_t length)
{
    zos_i2c_message_t msg = {result, length, I2C_MSG_READ, 0};

    return zn_i2c_transfer(&ZOS_I2C_HIH6130, &msg, 1);
}

/* Trigger measurement and read results, writes 4 bytes to data pointer*/
static zos_result_t measure_and_read(uint8_t *data, uint16_t length)
{
    //just need to issue slave address and write, reg value doesnt matter
    return zn_i2c_master_read_reg(&ZOS_I2C_HIH6130, 0, data, length);
}

/* Input data pointer, length to read, ZOS_TRUE to get last measurement
 * Returns result and outputs data */
static zos_result_t get_data(uint8_t* data, uint16_t length, zos_bool_t use_last)
{
    zos_result_t result;

    if(use_last == ZOS_TRUE)
    {
        result = read_data(data, length);
    }
    else
    {
        result = measure_and_read(data, length);
    }

    return result;
}

/* Input 4-byte result from reading sensor data, returns temperature value (raw) */
static uint16_t get_temperature(const uint8_t* data)
{
    return GET_TEMPERATURE(data[2], data[3]);
}

/* Input 4-byte result from reading sensor data, returns temperature value (raw) */
static uint16_t get_humidity(const uint8_t* data)
{
    return GET_HUMIDITY(data[0], data[1]);
}

/* Input 4-byte result from reading sensor data, returns temperature value (raw) */
static uint16_t get_status(const uint8_t* data)
{
    return GET_STATUS(data[0]);
}
