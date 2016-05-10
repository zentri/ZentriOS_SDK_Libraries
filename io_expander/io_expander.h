/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 */
/** @file
 */
#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include "zos.h"


/**
 * @addtogroup  lib_peripherals_io_expander
 * @{
 */



#define IO_EXPANDER_VAL_LOW             0U
#define IO_EXPANDER_VAL_HIGH            1U

#define IO_EXPANDER_DIR_OUT             0U
#define IO_EXPANDER_DIR_IN              1U

#define IO_EXPANDER_POL_ACTIVE_HIGH     0U
#define IO_EXPANDER_POL_ACTIVE_LOW      1U


typedef struct
{
	uint16_t bank_id;
	uint8_t bitmask;
} io_expander_pin_t;

typedef uint8_t io_expander_val_t;
typedef uint8_t io_expander_dir_t;
typedef uint8_t io_expander_pol_t;

typedef zos_result_t (*get_input_values_prototype_t)(const zos_i2c_device_t* i2c, uint8_t* result);
typedef zos_result_t (*get_output_values_prototype_t)(const zos_i2c_device_t* i2c, uint8_t* result);
typedef zos_result_t (*set_values_prototype_t)(const zos_i2c_device_t* i2c, const uint8_t* values);
typedef zos_result_t (*get_directions_prototype_t)(const zos_i2c_device_t* i2c, uint8_t* result);
typedef zos_result_t (*set_directions_prototype_t)(const zos_i2c_device_t* i2c, const uint8_t* directions);
typedef zos_result_t (*get_polarities_prototype_t)(const zos_i2c_device_t* i2c, uint8_t* result);
typedef zos_result_t (*set_polarities_prototype_t)(const zos_i2c_device_t* i2c, const uint8_t* polarities);

typedef struct
{
	const uint16_t num_banks;
	const zos_i2c_device_t* device;
} io_expander_device_t;


/**
 * Get values of IO expander port
 *
 * @param[in] dev: platform IO expander to use
 * @param[out] result: Array of pin input values of device
 * @return @ref zos_result_t
 */
zos_result_t io_expander_val_get_all(const io_expander_device_t *dev, io_expander_val_t *result);

/**
 * Set values of platform_io_exp port
 *
 * @param[in] dev: platform IO expander to use
 * @param[in] values : Array of Pin output values to set
 * @return @ref zos_result_t
 */
zos_result_t io_expander_val_set_all(const io_expander_device_t *dev, const io_expander_dir_t *values);

/**
 * Get directions of platform_io_exp port
 *
 * @param[in] dev: platform IO expander to use
 * @param[out] result: Pin directions
 * @return @ref zos_result_t
 *
 */
zos_result_t io_expander_dir_get_all(const io_expander_device_t *dev, io_expander_dir_t *result);

/**
 * Set directions of platform_io_exp port
 *
 * @param[in] dev: platform IO expander to use
 * @param[in] directions : Pin directions to set
 * @return @ref zos_result_t
 */
zos_result_t io_expander_dir_set_all(const io_expander_device_t *dev, const io_expander_dir_t *directions);

/**
 * Get polarities of platform_io_exp port
 *
 * @param[in] dev: platform IO expander to use
 * @param[out] result: Pin polarities
 * @return @ref zos_result_t
 */
zos_result_t io_expander_pol_get_all(const io_expander_device_t *dev, io_expander_pol_t *result);

/**
 * Set polarities of platform_io_exp port
 *
 * @param[in] dev: platform IO expander to use
 * @param[in] polarities : Pin polarities to set
 * @return @ref zos_result_t
 */
zos_result_t io_expander_pol_set_all(const io_expander_device_t *dev, const io_expander_pol_t *polarities);


/**
 * Get value of IO expander port
 *
 * @param[in] dev: Platform IO expander to use
 * @param[in] pin: Pin to read
 * @param[out] result: Input value of device
 * @return @ref zos_result_t
 */
zos_result_t io_expander_val_get(const io_expander_device_t* dev, const io_expander_pin_t *pin, io_expander_val_t *result);

/**
 * Set value of platform_io_exp port pin
 *
 * @param[in] dev: platform IO expander to use
 * @param[in] pin: Pin to write
 * @param[in] value : Pin output value to set
 * @return @ref zos_result_t
 */
zos_result_t io_expander_val_set(const io_expander_device_t *dev, const io_expander_pin_t *pin, const io_expander_val_t value);

/**
 * Get direction of platform_io_exp port pin
 *
 * @param[in] dev: platform IO expander to use
 * @param[in] pin: Pin to read
 * @param[out] result: Pin directions
 * @return @ref zos_result_t
 *
 */
zos_result_t io_expander_dir_get(const io_expander_device_t *dev, const io_expander_pin_t *pin, io_expander_dir_t *result);

/**
 * Set direction of platform_io_exp port pin
 *
 * @param[in] dev: platform IO expander to use
 * @param[in] pin: Pin to write
 * @param[in] direction : Pin direction to set
 * @return @ref zos_result_t
 */
zos_result_t io_expander_dir_set(const io_expander_device_t *dev, const io_expander_pin_t *pin, const io_expander_dir_t direction);

/**
 * Get polarity of platform_io_exp port
 *
 * @param[in] dev: platform IO expander to use
 * @param[in] pin: Pin to read
 * @param[out] result: Pin polarity
 * @return @ref zos_result_t
 */
zos_result_t io_expander_pol_get(const io_expander_device_t *dev, const io_expander_pin_t *pin, io_expander_pol_t *result);

/**
 * Set polarity of platform_io_exp port
 *
 * @param[in] dev: platform IO expander to use
 * @param[in] pin: Pin to write
 * @param[in] polarity : Pin polarity to set
 * @return @ref zos_result_t
 */
zos_result_t io_expander_pol_set(const io_expander_device_t *dev, const io_expander_pin_t *pin,const io_expander_pol_t polarity);
#ifdef __cplusplus
} /*extern "C" */
#endif

/**
 * @}
 */
