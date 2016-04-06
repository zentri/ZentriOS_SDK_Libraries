/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include <stdlib.h>
#include "tca6416.h"
#include "zos.h"

#ifdef DRIVER_TCA6416

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define TCA6416_REG_SIZE (uint16_t)2U
/******************************************************
 *                   Enumerations
 ******************************************************/
typedef enum
{
    REG_INPUT0=0,
    REG_INPUT1,
    REG_OUTPUT0,
    REG_OUTPUT1,
    REG_POLARITY0,
    REG_POLARITY1,
    REG_CONFIG0,
    REG_CONFIG1
} tca6416_register_t;
/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
#ifdef DEBUG
static zos_bool_t is_device_valid(const zos_i2c_device_t* i2c);
static zos_bool_t args_valid(const zos_i2c_device_t* i2c, uint8_t* result);
#else
#define is_device_valid(x) 1
#define args_valid(x, y) 1
#endif
/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *          External Function Definitions
 ******************************************************/

zos_result_t tca6416_get_input_values(const zos_i2c_device_t* i2c, uint8_t* result)
{
    zos_result_t cmd_result = ZOS_INVALID_ARG;

    if (args_valid(i2c, result))
    {
        cmd_result = zn_i2c_master_read_reg(i2c, REG_INPUT0, result,  TCA6416_REG_SIZE);
    }

    return cmd_result;
}

zos_result_t tca6416_get_output_values(const zos_i2c_device_t* i2c, uint8_t* result)
{
    zos_result_t cmd_result = ZOS_INVALID_ARG;

    if (args_valid(i2c, result))
    {
        cmd_result = zn_i2c_master_read_reg(i2c, REG_OUTPUT0, result, TCA6416_REG_SIZE);
    }

    return cmd_result;
}

zos_result_t tca6416_set_values(const zos_i2c_device_t* i2c, const uint8_t* values)
{
    zos_result_t cmd_result = ZOS_INVALID_ARG;

    if (is_device_valid(i2c))
    {
        cmd_result = zn_i2c_master_write_reg(i2c, REG_OUTPUT0, values,  TCA6416_REG_SIZE);
    }

    return cmd_result;
}

zos_result_t tca6416_get_directions(const zos_i2c_device_t* i2c, uint8_t* result)
{
    zos_result_t cmd_result = ZOS_INVALID_ARG;

    if (args_valid(i2c, result))
    {
        cmd_result = zn_i2c_master_read_reg(i2c, REG_CONFIG0, result, TCA6416_REG_SIZE);
    }

    return cmd_result;
}

zos_result_t tca6416_set_directions(const zos_i2c_device_t* i2c, const uint8_t* directions)
{
    zos_result_t cmd_result = ZOS_INVALID_ARG;

    if (is_device_valid(i2c))
    {
        cmd_result = zn_i2c_master_write_reg(i2c, REG_CONFIG0, directions, TCA6416_REG_SIZE);
    }

    return cmd_result;
}

zos_result_t tca6416_get_polarities(const zos_i2c_device_t* i2c, uint8_t* result)
{
    zos_result_t cmd_result = ZOS_INVALID_ARG;

    if (args_valid(i2c, result))
    {
        cmd_result = zn_i2c_master_read_reg(i2c, REG_POLARITY0, result, TCA6416_REG_SIZE);
    }

    return cmd_result;
}

zos_result_t tca6416_set_polarities(const zos_i2c_device_t* i2c, const uint8_t* polarities)
{
    zos_result_t cmd_result = ZOS_INVALID_ARG;

    if (is_device_valid(i2c))
    {
        cmd_result = zn_i2c_master_write_reg(i2c, REG_POLARITY0, polarities, TCA6416_REG_SIZE);
    }

    return cmd_result;
}

/******************************************************
 *          Internal Function Definitions
 ******************************************************/
#ifdef DEBUG
static zos_bool_t args_valid(const zos_i2c_device_t* i2c, uint8_t* result)
{
    return (zos_bool_t)(is_device_valid(i2c) && (result != NULL));
}

static zos_bool_t is_device_valid(const zos_i2c_device_t* i2c)
{
    return (zos_bool_t)(i2c != NULL);
}
#endif


#endif
