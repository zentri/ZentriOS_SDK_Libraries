/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once
#include "zos.h"

/* WICED Sense Service UUID: 739298B6-87B6-4984-A5DC-BDC18B068985 */
#define WICED_SENSE_UUID_SERVICE  ZOS_BT_UUID_INIT_128( 0x85, 0x89, 0x06, 0x8b, 0xc1, 0xbd, 0xdc, 0xa5, 0x84, 0x49, 0xb6, 0x87, 0xb6, 0x98, 0x92, 0x73 )

/* WICED Sense Data Characteristic: 33EF9113-3B55-413E-B553-FEA1EAADA459 */
#define WICED_SENSE_UUID_CHARACTERISTIC_DATA  ZOS_BT_UUID_INIT_128( 0x59, 0xa4, 0xad, 0xea, 0xa1, 0xfe, 0x53, 0xb5, 0x3e, 0x41, 0x55, 0x3b, 0x13, 0x91, 0xef, 0x33 )

#define WICED_SENSE2_ACCELEROMETER_DATA_AVAILABLE  ( 1 << 0 ) // Bit 0: When set, accelerometer included X (2 signed bytes), Y (2 signed bytes) and Z (2 signed bytes).
#define WICED_SENSE2_GYRO_DATA_AVAILABLE           ( 1 << 1 ) // Bit 1: When set, gyro included X (2 signed bytes), Y (2 signed bytes) and Z (2 signed bytes).
#define WICED_SENSE2_HUMIDITY_DATA_AVAILABLE       ( 1 << 2 ) // Bit 2: When set, humidity included (2 unsigned bytes in percentage).
#define WICED_SENSE2_MAGNETOMETER_DATA_AVAILABLE   ( 1 << 3 ) // Bit 3: When set, magnetometer included X (2 signed bytes), Y (2 signed bytes) and Z (2 signed bytes).
#define WICED_SENSE2_PRESSURE_DATA_AVAILABLE       ( 1 << 4 ) // Bit 4: When set, pressure included (2 bytes, pressure in mBar * 10)
#define WICED_SENSE2_TEMPERATURE_DATA_AVAILABLE    ( 1 << 5 ) // Bit 5: When set, temperature included (2 unsigned bytes, temperature in C * 10).

#define ACCELERATOR_RANGE_MAX_G ( 8 )   /* min/max -/+8G */
#define GYRO_ROTATION_MAX_DEG   ( 250 ) /* min/max -/+250 */
#define MAGNETOMETER_MAX_GAUSS  ( 8 )   /* min/max -/+8 gauss */

typedef enum
{
    WICED_SENSE2_DATA_1 = ( WICED_SENSE2_ACCELEROMETER_DATA_AVAILABLE | WICED_SENSE2_GYRO_DATA_AVAILABLE | WICED_SENSE2_MAGNETOMETER_DATA_AVAILABLE ),
    WICED_SENSE2_DATA_2 = ( WICED_SENSE2_HUMIDITY_DATA_AVAILABLE | WICED_SENSE2_PRESSURE_DATA_AVAILABLE | WICED_SENSE2_TEMPERATURE_DATA_AVAILABLE ),
    WICED_SENSE2_DATA_MAX = 0x7f,
} wiced_sense2_data_type_t;

#pragma pack(1)
typedef struct
{
    int16_t x_acceleration;
    int16_t y_acceleration;
    int16_t z_acceleration;
    int16_t x_rotation;
    int16_t y_rotation;
    int16_t z_rotation;
    int16_t x_magnetic_field;
    int16_t y_magnetic_field;
    int16_t z_magnetic_field;
} wiced_sense2_raw_data1_t;

typedef struct
{
    uint16_t humidity;
    uint16_t pressure;
    uint16_t temperature;
} wiced_sense2_raw_data2_t;

typedef struct
{
    wiced_sense2_data_type_t type;
    union
    {
        wiced_sense2_raw_data1_t data1;
        wiced_sense2_raw_data2_t data2;
    } data;
} wiced_sense2_raw_data_t;
#pragma pack()

typedef struct
{
    fpi_word_t x_rotation_degree_per_second;
    fpi_word_t y_rotation_degree_per_second;
    fpi_word_t z_rotation_degree_per_second;
    fpi_word_t x_acceleration_g;
    fpi_word_t y_acceleration_g;
    fpi_word_t z_acceleration_g;
    fpi_word_t x_magnetic_field_gauss;
    fpi_word_t y_magnetic_field_gauss;
    fpi_word_t z_magnetic_field_gauss;
} wiced_sense2_data1_t;

typedef struct
{
    fpi_word_t humidity_percent;
    fpi_word_t pressure_hpa;
    fpi_word_t temperature_celcius;
} wiced_sense2_data2_t;

typedef struct
{
    wiced_sense2_data_type_t type;
    union
    {
        wiced_sense2_data1_t data1;
        wiced_sense2_data2_t data2;
    } data;
} wiced_sense2_data_t;

/**
 * Convert WICED sense raw data into usable data formats
 *
 * @param[in]  raw_data Raw data
 * @param[out] data     Formatted data
 */
zos_result_t wiced_sense2_convert_data( const wiced_sense2_raw_data_t* raw_data, wiced_sense2_data_t* data );
