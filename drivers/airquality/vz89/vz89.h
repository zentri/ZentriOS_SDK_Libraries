/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once



#include "zos.h"

typedef struct
{
    uint8_t co2;
    uint8_t voc_short;
    uint8_t voc_long;
    uint8_t raw_resistor1;
    uint8_t raw_resistor2;
    uint8_t raw_resistor3;
} sensor_data_t;

zos_result_t vz89_airquality_read(sensor_data_t *raw_quality);
