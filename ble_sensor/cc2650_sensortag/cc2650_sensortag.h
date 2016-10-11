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

/* Motion sensor service UUID: F000AA00-0451-4000-B000-000000000000 */
#define CC2650_UUID_SERVICE_TEMP_SENSOR                   ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x00, 0xAA, 0x00, 0xF0 )

/* Motion sensor data characteristic UUID: F000AA01-0451-4000-B000-000000000000 */
#define CC2650_UUID_CHARACTERISTIC_TEMP_SENSOR_DATA       ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x01, 0xAA, 0x00, 0xF0 )

/* Motion sensor configuration characteristic UUID: F000AA02-0451-4000-B000-000000000000 */
#define CC2650_UUID_CHARACTERISTIC_TEMP_SENSOR_CONFIG     ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x02, 0xAA, 0x00, 0xF0 )
#define CC2650_TEMP_SENSOR_DISABLE                        0x00
#define CC2650_TEMP_SENSOR_ENABLE                         0x01

/* Motion sensor period characteristic UUID: F000AA03-0451-4000-B000-000000000000 */
#define CC2650_UUID_CHARACTERISTIC_TEMP_SENSOR_PERIOD     ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x03, 0xAA, 0x00, 0xF0 )
#define CC2650_TEMP_SENSOR_PERIOD_MIN                     0x1E /* 30  * 10ms = 300ms */
#define CC2650_TEMP_SENSOR_PERIOD_MAX                     0xFF /* 255 * 10ms = 2.55 seconds */
#define CC2650_TEMP_SENSOR_PERIOD_DEFAULT                 0x64 /* 100 * 10ms = 1 second */

/* Motion Sensor Service UUID: F000AA80-0451-4000-B000-000000000000 */
#define CC2650_UUID_SERVICE_MOTION_SENSOR                 ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x80, 0xAA, 0x00, 0xF0 )

/* Motion Sensor Data Characteristic UUID: F000AA81-0451-4000-B000-000000000000 */
#define CC2650_UUID_CHARACTERISTIC_MOTION_SENSOR_DATA     ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x81, 0xAA, 0x00, 0xF0 )

/* Motion Sensor Configuration Characteristic UUID: F000AA82-0451-4000-B000-000000000000 */
#define CC2650_UUID_CHARACTERISTIC_MOTION_SENSOR_CONFIG   ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x82, 0xAA, 0x00, 0xF0 )
#define CC2650_MOTION_SENSOR_GYRO_X_ENABLE                ( 1 << 0 )
#define CC2650_MOTION_SENSOR_GYRO_Y_ENABLE                ( 1 << 1 )
#define CC2650_MOTION_SENSOR_GYRO_Z_ENABLE                ( 1 << 2 )
#define CC2650_MOTION_SENSOR_ACCELLEROMETER_X_ENABLE      ( 1 << 3 )
#define CC2650_MOTION_SENSOR_ACCELLEROMETER_Y_ENABLE      ( 1 << 4 )
#define CC2650_MOTION_SENSOR_ACCELLEROMETER_Z_ENABLE      ( 1 << 5 )
#define CC2650_MOTION_SENSOR_MAGNETOMETER_ENABLE          ( 1 << 6 ) /* All axis */
#define CC2650_MOTION_SENSOR_WAKE_ON_MOTION_ENABLE        ( 1 << 7 )
#define CC2650_MOTION_SENSOR_ACCELLEROMETER_RANGE_2G      ( 0 << 8 )
#define CC2650_MOTION_SENSOR_ACCELLEROMETER_RANGE_4G      ( 1 << 8 )
#define CC2650_MOTION_SENSOR_ACCELLEROMETER_RANGE_8G      ( 2 << 8 )
#define CC2650_MOTION_SENSOR_ACCELLEROMETER_RANGE_16G     ( 3 << 8 )

/* Humidity Sensor Service UUID: F000AA20-0451-4000-B000-000000000000 */
#define CC2650_UUID_SERVICE_HUMIDITY_SENSOR               ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x20, 0xAA, 0x00, 0xF0 )

/* Humidity Sensor Data Characteristic UUID: F000AA21-0451-4000-B000-000000000000 */
#define CC2650_UUID_CHARACTERISTIC_HUMIDITY_SENSOR_DATA   ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x21, 0xAA, 0x00, 0xF0 )

/* Humidity Sensor Configuration Characteristic UUID: F000AA22-0451-4000-B000-000000000000 */
#define CC2650_UUID_CHARACTERISTIC_HUMIDITY_SENSOR_CONFIG ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x22, 0xAA, 0x00, 0xF0 )
#define CC2650_HUMIDITY_SENSOR_DISABLE                    0x00
#define CC2650_HUMIDITY_SENSOR_ENABLE                     0x01

/* Optical Sensor Service UUID: F000AA70-0451-4000-B000-000000000000 */
#define CC2650_UUID_SERVICE_OPTICAL_SENSOR                ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x70, 0xAA, 0x00, 0xF0 )

/* Optical Sensor Data Characteristic UUID: F000AA71-0451-4000-B000-000000000000 */
#define CC2650_UUID_CHARACTERISTIC_OPTICAL_SENSOR_DATA    ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x71, 0xAA, 0x00, 0xF0 )

/* Optical Sensor Configuration Characteristic UUID: F000AA72-0451-4000-B000-000000000000 */
#define CC2650_UUID_CHARACTERISTIC_OPTICAL_SENSOR_CONFIG  ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x72, 0xAA, 0x00, 0xF0 )
#define CC2650_OPTICAL_SENSOR_DISABLE                     0x00
#define CC2650_OPTICAL_SENSOR_ENABLE                      0x01

/* IO Service UUID: F000AA64-0451-4000-B000-000000000000 */
#define CC2650_UUID_SERVICE_IO                            ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x64, 0xAA, 0x00, 0xF0 )

/* IO Data Characteristic UUID: F000AA65-0451-4000-B000-000000000000 */
#define CC2650_UUID_CHARACTERISTIC_IO_DATA                ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x65, 0xAA, 0x00, 0xF0 )

/* IO Configuration UUID: F000AA66-0451-4000-B000-000000000000 */
#define CC2650_UUID_CHARACTERISTIC_IO_CONFIG              ZOS_BT_UUID_INIT_128( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x66, 0xAA, 0x00, 0xF0 )

/* Simple Keys (Buttons) Service UUID: 0xFFE0 */
#define CC2650_UUID_SERVICE_SIMPLE_KEYS                   ZOS_BT_UUID_INIT_16( 0xFFE0 )

/* Simple Keys Data Characteristic UUID: 0xFFE1 */
#define CC2650_UUID_CHARACTERISTIC_SIMPLE_KEYS_DATA       ZOS_BT_UUID_INIT_16( 0xFFE1 )

#define ACCELERATOR_RANGE_MAX_G ( 8 ) /* min/max -/+8G */

#pragma pack(1)
typedef struct
{
    uint16_t ir_temperature;
    uint16_t ambient_temperature;
} cc2650_temperature_sensor_raw_data_t;

typedef struct
{
    fpi_word_t ir_temperature_celcius;
    fpi_word_t ambient_temperature_celcius;
} cc2650_temperature_sensor_data_t;

typedef struct
{
    uint16_t temperature;
    uint16_t relative_humidity;
} cc2650_humidity_sensor_raw_data_t;

typedef struct
{
    fpi_word_t temperature_celcius;
    fpi_word_t relative_humidity_percent;
} cc2650_humidity_sensor_data_t;

typedef struct
{
    uint8_t temperature[3];
    uint8_t pressure[3];
} cc2650_barometer_raw_data_t;

typedef struct
{
    fpi_word_t temperature_celcius;
    fpi_word_t barometric_pressure_hpa;
} cc2650_barometer_data_t;

typedef struct
{
    int16_t x_rotation;
    int16_t y_rotation;
    int16_t z_rotation;
    int16_t x_acceleration;
    int16_t y_acceleration;
    int16_t z_acceleration;
    int16_t x_magnetic_field;
    int16_t y_magnetic_field;
    int16_t z_magnetic_field;
} cc2650_motion_sensor_raw_data_t;

typedef struct
{
    fpi_word_t x_rotation_degree_per_second;
    fpi_word_t y_rotation_degree_per_second;
    fpi_word_t z_rotation_degree_per_second;
    fpi_word_t x_acceleration_g;
    fpi_word_t y_acceleration_g;
    fpi_word_t z_acceleration_g;
    fpi_word_t x_magnetic_field_microtesla;
    fpi_word_t y_magnetic_field_microtesla;
    fpi_word_t z_magnetic_field_microtesla;
} cc2650_motion_sensor_data_t;
#pragma pack()

/**
 * Convert CC2650 temperature sensor raw data into usable data formats
 *
 * @param[in]  raw_data       Raw data
 * @param[out] formatted_data Formatted data
 */
void cc2650_temperature_sensor_convert_data( const cc2650_temperature_sensor_raw_data_t* raw_data, cc2650_temperature_sensor_data_t* formatted_data );

/**
 * Convert CC2650 humidity sensor raw data into usable data formats
 *
 * @param[in]  raw_data       Raw data
 * @param[out] formatted_data Formatted data
 */
void cc2650_humidity_sensor_convert_data( const cc2650_humidity_sensor_raw_data_t* raw_data, cc2650_humidity_sensor_data_t* formatted_data );

/**
 * Convert CC2650 ambient light sensor raw data into usable data formats
 *
 * @param[in]  raw_data            Raw data
 * @param[out] light_intensity_lux Light intensity in Lux
 */
void cc2650_ambient_light_sensor_convert_data( uint16_t raw_data, fpi_word_t* light_intensity_lux );

/**
 * Convert CC2650 barometer raw data into usable data formats
 *
 * @param[in]  raw_data       Raw data
 * @param[out] formatted_data Formatted data
 */
void cc2650_barometer_convert_data( const cc2650_barometer_raw_data_t* raw_data, cc2650_barometer_data_t* formatted_data );

/**
 * Convert CC2650 motion raw data into usable data formats
 *
 * @param[in]  raw_data       Raw data
 * @param[out] formatted_data Formatted data
 */
void cc2650_motion_sensor_convert_data( const cc2650_motion_sensor_raw_data_t* raw_data, cc2650_motion_sensor_data_t* formatted_data );
