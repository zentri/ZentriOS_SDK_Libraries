/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "cc2650_sensortag.h"
#include "math.h"

void cc2650_temperature_sensor_convert_data( const cc2650_temperature_sensor_raw_data_t* raw_data, cc2650_temperature_sensor_data_t* formatted_data )
{
    const fpi_word_t scale_lsb = fpi_rconst( 0.03125 );

    formatted_data->ir_temperature_celcius = fpi_fromint( ( raw_data->ir_temperature ) >> 2 );
    fpi_multiply( (const fpi_word_t*)&formatted_data->ir_temperature_celcius, &scale_lsb, &formatted_data->ir_temperature_celcius );
    formatted_data->ambient_temperature_celcius = fpi_fromint( ( raw_data->ambient_temperature ) >> 2 );
    fpi_multiply( (const fpi_word_t*)&formatted_data->ambient_temperature_celcius, &scale_lsb, &formatted_data->ambient_temperature_celcius );
}

void cc2650_humidity_sensor_convert_data( const cc2650_humidity_sensor_raw_data_t* raw_data, cc2650_humidity_sensor_data_t* formatted_data )
{
    const fpi_word_t divider = fpi_rconst( 65536 );
    fpi_word_t multiplier;

    /* calculate temperature [°C] */
    formatted_data->temperature_celcius = fpi_fromint( raw_data->temperature );
    fpi_divide( (const fpi_word_t*)&formatted_data->temperature_celcius, (const fpi_word_t*)&divider, &formatted_data->temperature_celcius );
    multiplier = fpi_rconst( 165 );
    fpi_multiply( (const fpi_word_t*)&formatted_data->temperature_celcius, &multiplier, &formatted_data->temperature_celcius );
    formatted_data->temperature_celcius = fpi_sub( formatted_data->temperature_celcius, fpi_fromint( 40 ) );

    /* calculate relative humidity [%RH] */
    formatted_data->relative_humidity_percent = fpi_fromint( raw_data->relative_humidity );
    fpi_divide( (const fpi_word_t*)&formatted_data->relative_humidity_percent, (const fpi_word_t*)&divider, &formatted_data->relative_humidity_percent );
    multiplier = fpi_rconst( 100 );
    fpi_multiply( (const fpi_word_t*)&formatted_data->relative_humidity_percent, &multiplier, &formatted_data->relative_humidity_percent );
}

void cc2650_ambient_light_sensor_convert_data( uint16_t raw_data, fpi_word_t* light_intensity_lux )
{
    float e;
    uint16_t m;

    m = raw_data & 0x0FFF;
    e = (float) ( ( raw_data & 0xF000 ) >> 12 );
    *light_intensity_lux = fpi_rconst( ( m * ( 0.01 * pow( 2.0, e ) ) ) );
}

void cc2650_barometer_convert_data( const cc2650_barometer_raw_data_t* raw_data, cc2650_barometer_data_t* formatted_data )
{
    const fpi_word_t constant = fpi_rconst( 100.0 );
    uint32_t temperature_raw_uint32 = 0;
    uint32_t pressure_raw_uint32    = 0;

    memcpy( &temperature_raw_uint32, raw_data->temperature, sizeof( raw_data->temperature ) );
    memcpy( &pressure_raw_uint32,    raw_data->pressure,    sizeof( raw_data->pressure    ) );

    formatted_data->temperature_celcius     = fpi_fromint( temperature_raw_uint32 );
    formatted_data->barometric_pressure_hpa = fpi_fromint( pressure_raw_uint32 );

    fpi_divide ( (const fpi_word_t*)&formatted_data->temperature_celcius    , &constant,  &formatted_data->temperature_celcius     );
    fpi_divide ( (const fpi_word_t*)&formatted_data->barometric_pressure_hpa,  &constant, &formatted_data->barometric_pressure_hpa );
}

void cc2650_motion_sensor_convert_data( const cc2650_motion_sensor_raw_data_t* raw_data, cc2650_motion_sensor_data_t* formatted_data )
{
    fpi_word_t constant;

    /* calculate rotation, unit deg/s, range -250, +250 */
    constant = fpi_rconst( ( 65636 / 500 ) );
    formatted_data->x_rotation_degree_per_second = fpi_fromint( raw_data->x_rotation );
    fpi_divide( (const fpi_word_t*)&formatted_data->x_rotation_degree_per_second, (const fpi_word_t*)&constant, &formatted_data->x_rotation_degree_per_second );
    formatted_data->y_rotation_degree_per_second = fpi_fromint( raw_data->y_rotation );
    fpi_divide( (const fpi_word_t*)&formatted_data->y_rotation_degree_per_second, (const fpi_word_t*)&constant, &formatted_data->y_rotation_degree_per_second );
    formatted_data->z_rotation_degree_per_second = fpi_fromint( raw_data->z_rotation );
    fpi_divide( (const fpi_word_t*)&formatted_data->z_rotation_degree_per_second, (const fpi_word_t*)&constant, &formatted_data->z_rotation_degree_per_second );

    /* calculate acceleration, unit G, range -8, +8 */
    constant = fpi_rconst( ( 32768 / ACCELERATOR_RANGE_MAX_G ) );
    formatted_data->x_acceleration_g = fpi_fromint( raw_data->x_acceleration );
    fpi_divide( (const fpi_word_t*)&formatted_data->x_acceleration_g, &constant, &formatted_data->x_acceleration_g );
    formatted_data->y_acceleration_g = fpi_fromint( raw_data->y_acceleration );
    fpi_divide( (const fpi_word_t*)&formatted_data->y_acceleration_g, &constant, &formatted_data->y_acceleration_g );
    formatted_data->z_acceleration_g = fpi_fromint( raw_data->z_acceleration );
    fpi_divide( (const fpi_word_t*)&formatted_data->z_acceleration_g, &constant, &formatted_data->z_acceleration_g );

    /* calculate magnetism, unit uT, range +-4900 */
    constant = fpi_rconst ( 0.15 );
    formatted_data->x_magnetic_field_microtesla = fpi_fromint( raw_data->x_magnetic_field );
    fpi_multiply( (const fpi_word_t*)&formatted_data->x_magnetic_field_microtesla, &constant, &formatted_data->x_magnetic_field_microtesla );
    formatted_data->y_magnetic_field_microtesla = fpi_fromint( raw_data->y_magnetic_field );
    fpi_multiply( (const fpi_word_t*)&formatted_data->y_magnetic_field_microtesla, &constant, &formatted_data->y_magnetic_field_microtesla );
    formatted_data->z_magnetic_field_microtesla = fpi_fromint( raw_data->z_magnetic_field );
    fpi_multiply( (const fpi_word_t*)&formatted_data->z_magnetic_field_microtesla, &constant, &formatted_data->z_magnetic_field_microtesla );
}
