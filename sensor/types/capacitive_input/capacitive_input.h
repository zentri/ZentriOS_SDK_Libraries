/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once

#include <stdint.h>
#include "zos.h"

typedef enum
{
	MODCDC1_CDC_RANGE_2_PF = 0,
	MODCDC1_CDC_RANGE_0_5_PF,
	MODCDC1_CDC_RANGE_1_PF,
	MODCDC1_CDC_RANGE_4_PF,
}cdc_input_range_t;

typedef enum
{
	ADAPTIVE_THRESHOLD_MODE = 0,
	FIXED_THRESHOLD_MODE,
}cdc_threshold_mode_t;

typedef enum
{
	THRESHOLD_NEGATIVE = 0,
	THRESHOLD_POSITIVE,
	THRESHOLD_INWINDOW,
	THRESHOLD_OUTWINDOW,
}cdc_output_comparator_mode_t;

typedef enum
{
	MODE_IDLE = 0,
	MODE_CONTINUOUS_CONVERSION,
	MODE_SINGLE_CONVERSION,
	MODE_POWER_DOWN,
	MODE_RESERVED,
}cdc_operation_mode_t;

typedef struct
{
	cdc_operation_mode_t op_mode;
	uint8_t ch1_enable;
	uint8_t ch2_enable;
	cdc_threshold_mode_t threshold_mode;
	cdc_output_comparator_mode_t output_comparator_mode;
	cdc_input_range_t ch1_input_range;
	cdc_input_range_t ch2_input_range;
	float ch1_threshold;  // unit in Pf
	float ch2_threshold;  // unit in Pf
	float ch1_sensitivity; // unit in Pf
	float ch2_sensitivity; // unit in Pf
	uint8_t ch1_adaptive_timeout;
	uint8_t ch2_adaptive_timeout;
	zos_bool_t ch1_adapative_theresis_enabled;
	zos_bool_t ch2_adapative_theresis_enabled;
} capacitive_input_config_t;


/**
 * Capacitive input buttons data
 */
typedef struct
{
	int16_t ch1_capacitance;
	int16_t ch2_capacitance;
	float adjusted_ch1_cap;
	float adjusted_ch2_cap;
} capacitive_input_data_t;


/**
 * @}
 */

/**
 * Initialise capacitive input buttons driver.
 *
 * @param[in] config: config used by initialisation function
 * @return @ref zos_result_t
 */
zos_result_t sensor_capacitive_input_init(const capacitive_input_config_t *config);

/**
 * Poll capacitive input buttons to see if new data is available
 *
 * @param[out] has_data: True if there is new data available
 * @return @ref zos_result_t
 */
zos_result_t sensor_capacitive_input_has_new_data(zos_bool_t *has_data);

/**
 * Get latest data from capacitive input
 *
 * @param[out] data: Relative capacitance (0 - 100%) x 10
 * @return @ref zos_result_t
 */
zos_result_t sensor_capacitive_input_get_data(capacitive_input_data_t *data);

/**
 * Read raw data from sensor
 *
 * @param[out] raw_data: data data read from sensor
 * @param[in] use_last_value: use last read value instead of reading from sensor
 * @return @ref zos_result_t
 */
zos_result_t sensor_capacitive_input_read(uint16_t *raw_data);

/**
 * Convert raw data to normalized value
 * @param raw_value : raw data from @ref sensor_capacitive_input_read()
 * @return @ref zos_result_t
 */
float sensor_capacitive_input_convert(uint16_t raw_value);


