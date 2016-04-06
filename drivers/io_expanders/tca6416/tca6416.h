/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#ifndef tca6416_DRIVER_H_
#define tca6416_DRIVER_H_

#include <stdint.h>
#include "zos.h"


#define TCA6416_NUM_BANKS 2U



/**
 * Get values of tca6416 input port
 *
 * @param[in] i2c: I2C device to use
 * @param[out] result: Pin input values of tca6416 device
 * @return @ref zos_result_t
 */
zos_result_t tca6416_get_input_values(const zos_i2c_device_t* i2c, uint8_t* result);
/**
 * Get values of tca6416 output port
 *
 * @param[in] i2c: I2C device to use
 * @param[out] result: Pin output values of tca6416 device
 * @return @ref zos_result_t
 */
zos_result_t tca6416_get_output_values(const zos_i2c_device_t* i2c, uint8_t* result);

/**
 * Set values of tca6416 port
 *
 * @param[in] i2c: I2C device to use
 * @param[in] values : Pin output values to set
 * @return @ref zos_result_t
 */
zos_result_t tca6416_set_values(const zos_i2c_device_t* i2c, const uint8_t* values);

/**
 * Get directions of tca6416 port
 *
 * @param[in] i2c: I2C device to use
 * @param[out] result: Pin directions (1=Input, 0=Output)
 * @return @ref zos_result_t
 */
zos_result_t tca6416_get_directions(const zos_i2c_device_t* i2c, uint8_t* result);

/**
 * Set directions of tca6416 port
 *
 * @param[in] i2c: I2C device to use
 * @param[in] directions : Pin directions to set (1=Input, 0=Output)
 * @return @ref zos_result_t
 */
zos_result_t tca6416_set_directions(const zos_i2c_device_t* i2c, const uint8_t* directions);

/**
 * Get polarities of tca6416 port
 *
 * @param[in] i2c: I2C device to use
 * @param[out] result: Pin polarities
 * @return @ref zos_result_t
 */
zos_result_t tca6416_get_polarities(const zos_i2c_device_t* i2c, uint8_t* result);

/**
 * Set polarities of tca6416 port
 *
 * @param[in] i2c: I2C device to use
 * @param[in] polarities : Pin polarities to set (0=Active HIGH, 1=Active LOW)
 * @return @ref zos_result_t
 */
zos_result_t tca6416_set_polarities(const zos_i2c_device_t* i2c, const uint8_t* polarities);

#endif /* tca6416_DRIVER_H_ */
