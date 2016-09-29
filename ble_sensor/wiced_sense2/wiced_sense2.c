/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "wiced_sense2.h"

zos_result_t wiced_sense2_convert_data( const wiced_sense2_raw_data_t* raw_data, wiced_sense2_data_t* data )
{
    data->type = raw_data->type;

    if ( raw_data->type == WICED_SENSE2_DATA_1 )
    {
        fpi_word_t constant;

        /* calculate acceleration, unit G, range -8, +8 */
        constant = fpi_rconst( ( ( INT16_MAX + 1 ) / 100 ) );
        data->data.data1.x_acceleration_g = fpi_fromint( raw_data->data.data1.x_acceleration );
        fpi_divide( (const fpi_word_t*)&data->data.data1.x_acceleration_g, &constant, &data->data.data1.x_acceleration_g );
        data->data.data1.y_acceleration_g = fpi_fromint( raw_data->data.data1.y_acceleration );
        fpi_divide( (const fpi_word_t*)&data->data.data1.y_acceleration_g, &constant, &data->data.data1.y_acceleration_g );
        data->data.data1.z_acceleration_g = fpi_fromint( raw_data->data.data1.z_acceleration );
        fpi_divide( (const fpi_word_t*)&data->data.data1.z_acceleration_g, &constant, &data->data.data1.z_acceleration_g );

        /* calculate rotation, unit deg/s, range -250, +250 */
        constant = fpi_rconst( ( ( INT16_MAX + 1 ) / GYRO_ROTATION_MAX_DEG ) );
        data->data.data1.x_rotation_degree_per_second = fpi_fromint( raw_data->data.data1.x_rotation );
        fpi_divide( (const fpi_word_t*)&data->data.data1.x_rotation_degree_per_second, (const fpi_word_t*)&constant, &data->data.data1.x_rotation_degree_per_second );
        data->data.data1.y_rotation_degree_per_second = fpi_fromint( raw_data->data.data1.y_rotation );
        fpi_divide( (const fpi_word_t*)&data->data.data1.y_rotation_degree_per_second, (const fpi_word_t*)&constant, &data->data.data1.y_rotation_degree_per_second );
        data->data.data1.z_rotation_degree_per_second = fpi_fromint( raw_data->data.data1.z_rotation );
        fpi_divide( (const fpi_word_t*)&data->data.data1.z_rotation_degree_per_second, (const fpi_word_t*)&constant, &data->data.data1.z_rotation_degree_per_second );

        /* calculate magnetism, unit uT, range +-4900 */
        constant = fpi_rconst ( ( ( INT16_MAX + 1 ) / MAGNETOMETER_MAX_GAUSS ) );
        data->data.data1.x_magnetic_field_gauss = fpi_fromint( raw_data->data.data1.x_magnetic_field );
        fpi_divide( (const fpi_word_t*)&data->data.data1.x_magnetic_field_gauss, &constant, &data->data.data1.x_magnetic_field_gauss );
        data->data.data1.y_magnetic_field_gauss = fpi_fromint( raw_data->data.data1.y_magnetic_field );
        fpi_divide( (const fpi_word_t*)&data->data.data1.y_magnetic_field_gauss, &constant, &data->data.data1.y_magnetic_field_gauss );
        data->data.data1.z_magnetic_field_gauss = fpi_fromint( raw_data->data.data1.z_magnetic_field );
        fpi_divide( (const fpi_word_t*)&data->data.data1.z_magnetic_field_gauss, &constant, &data->data.data1.z_magnetic_field_gauss );

        return ZOS_SUCCESS;
    }
    else if ( raw_data->type == WICED_SENSE2_DATA_2 )
    {
        const fpi_word_t constant = fpi_rconst( 10.0 );
        uint32_t raw_uint32;

        /* calculate relative humidity [%RH] */
        raw_uint32 = 0;
        memcpy( &raw_uint32, &raw_data->data.data2.humidity, sizeof( raw_data->data.data2.humidity ) );
        data->data.data2.humidity_percent = fpi_fromint( raw_uint32 );
        fpi_divide ( (const fpi_word_t*)&data->data.data2.humidity_percent,  &constant, &data->data.data2.humidity_percent );

        /* calculate pressure [hPa] */
        raw_uint32 = 0;
        memcpy( &raw_uint32, &raw_data->data.data2.pressure, sizeof( raw_data->data.data2.pressure ) );
        data->data.data2.pressure_hpa = fpi_fromint( raw_uint32 );
        fpi_divide ( (const fpi_word_t*)&data->data.data2.pressure_hpa,  &constant, &data->data.data2.pressure_hpa );

        /* calculate pressure [degC] */
        raw_uint32 = 0;
        memcpy( &raw_uint32, &raw_data->data.data2.temperature, sizeof( raw_data->data.data2.temperature ) );
        data->data.data2.temperature_celcius = fpi_fromint( raw_uint32 );
        fpi_divide ( (const fpi_word_t*)&data->data.data2.temperature_celcius,  &constant, &data->data.data2.temperature_celcius );

        return ZOS_SUCCESS;
    }

    return ZOS_INVALID_ARG;
}
