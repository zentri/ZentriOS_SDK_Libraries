/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "io_expander.h"

#ifdef DRIVER_TCA6416
#include "tca6416.h"
#else
#error No IO Expander driver has been specified. e.g. GLOBAL_DEFINES := DRIVER_TCA6416
#endif


/* Returns ZOS_TRUE if dev and data arguments are valid */
#ifdef DEBUG
static zos_bool_t args_valid(const io_expander_device_t* dev, const void* data);
#else
#define args_valid(x, y) 1
#endif

/* Calcs bitmask for pin set functions based on a pins bank and bitmask */
//static uint16_t calc_bitmask(const io_expander_pin_t *pin);

/* Apply bitmask to values from register */
static void mask_values(uint8_t *reg_val, const uint8_t *val, const io_expander_pin_t* pin);

/* Get pointer to byte from register value according to bank of given pin */
static uint8_t* get_bank_value(const io_expander_pin_t* pin, uint8_t* values);

/* Set result value based on pin bitmask and bank value */
static void set_pin_result(uint8_t bitmask, uint8_t bank_value, uint8_t* result);

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *          External Function Definitions
 ******************************************************/
zos_result_t io_expander_val_get_all(const io_expander_device_t *dev, io_expander_val_t *result)
{
    zos_result_t get_result = ZOS_INVALID_ARG;

    if (args_valid(dev, result))
    {
#ifdef DRIVER_TCA6416
        get_result = tca6416_get_input_values(dev->device, (uint8_t*)result);
#endif
    }

    return get_result;
}


zos_result_t io_expander_val_set_all(const io_expander_device_t *dev, const io_expander_dir_t *values)
{
    zos_result_t ret_val = ZOS_INVALID_ARG;

    if (args_valid(dev, values))
    {
#ifdef DRIVER_TCA6416
        ret_val = tca6416_set_values(dev->device, (uint8_t*)values);
#endif
    }

    return ret_val;
}


zos_result_t io_expander_dir_get_all(const io_expander_device_t *dev, io_expander_dir_t *result)
{
    zos_result_t ret_val = ZOS_INVALID_ARG;

    if (args_valid(dev, result))
    {
#ifdef DRIVER_TCA6416
        ret_val = tca6416_get_directions(dev->device, (uint8_t*)result);
#endif
    }

    return ret_val;
}


zos_result_t io_expander_dir_set_all(const io_expander_device_t *dev, const io_expander_dir_t *directions)
{
    zos_result_t ret_val = ZOS_INVALID_ARG;

    if(args_valid(dev, directions))
    {
#ifdef DRIVER_TCA6416
        ret_val = tca6416_set_directions(dev->device, (uint8_t*)directions);
#endif
    }

    return ret_val;
}

zos_result_t io_expander_pol_get_all(const io_expander_device_t *dev, io_expander_pol_t *result)
{
    zos_result_t ret_val = ZOS_INVALID_ARG;

    if (args_valid(dev, result))
    {
#ifdef DRIVER_TCA6416
        ret_val = tca6416_get_polarities(dev->device, (uint8_t*)result);
#endif
    }

    return ret_val;
}

zos_result_t io_expander_pol_set_all(const io_expander_device_t *dev, const io_expander_pol_t *polarities)
{
    zos_result_t ret_val = ZOS_INVALID_ARG;

    if (args_valid(dev, polarities))
    {
#ifdef DRIVER_TCA6416
        ret_val = tca6416_set_polarities(dev->device, (uint8_t*)polarities);
#endif
    }

    return ret_val;
}

zos_result_t io_expander_val_get(const io_expander_device_t* dev, const io_expander_pin_t *pin, io_expander_val_t *result)
{
    zos_result_t ret_val = ZOS_INVALID_ARG;

    if (args_valid(dev, pin) && result != NULL)
    {
        uint8_t values[2];
#ifdef DRIVER_TCA6416
        ret_val = tca6416_get_input_values(dev->device, values);
#endif
        set_pin_result(pin->bitmask, *get_bank_value(pin, values), (uint8_t*)result);
    }

    return ret_val;
}

zos_result_t io_expander_val_set(const io_expander_device_t *dev, const io_expander_pin_t *pin, const io_expander_val_t value)
{
    zos_result_t ret_val = ZOS_INVALID_ARG;

    if (args_valid(dev, pin))
    {
#ifdef DRIVER_TCA6416
        uint8_t values[2];
        if (tca6416_get_output_values(dev->device, values) != ZOS_SUCCESS)
        {
            ret_val = ZOS_ERROR;
        }
        else
        {
            mask_values(values, (uint8_t*)&value, pin);
            ret_val = tca6416_set_values(dev->device, values);
        }
#endif
    }

    return ret_val;
}

zos_result_t io_expander_dir_get(const io_expander_device_t *dev, const io_expander_pin_t *pin, io_expander_dir_t *result)
{
    zos_result_t ret_val = ZOS_INVALID_ARG;

    if (args_valid(dev, pin) && result != NULL)
    {
        uint8_t dirs[2];
#ifdef DRIVER_TCA6416
        ret_val = tca6416_get_directions(dev->device, dirs);
#endif
        set_pin_result(pin->bitmask, *get_bank_value(pin, dirs), (uint8_t*)result);
    }

    return ret_val;
}

zos_result_t io_expander_dir_set(const io_expander_device_t *dev, const io_expander_pin_t *pin, const io_expander_dir_t direction)
{
    zos_result_t ret_val = ZOS_INVALID_ARG;

    if (args_valid(dev, pin))
    {
        uint8_t dirs[2];
#ifdef DRIVER_TCA6416
        if (tca6416_get_directions(dev->device, dirs) != ZOS_SUCCESS)
        {
            ret_val = ZOS_ERROR;
        }
        else
        {
            mask_values(dirs, (uint8_t*)&direction, pin);
            ret_val = tca6416_set_directions(dev->device, dirs);
        }
#endif
    }

    return ret_val;
}

zos_result_t io_expander_pol_get(const io_expander_device_t *dev, const io_expander_pin_t *pin, io_expander_pol_t *result)
{
    zos_result_t ret_val = ZOS_INVALID_ARG;

    if (dev != NULL && pin != NULL && result != NULL)
    {
        uint8_t pols[2];
#ifdef DRIVER_TCA6416
        ret_val = tca6416_get_polarities(dev->device, pols);
#endif
        set_pin_result(pin->bitmask, *get_bank_value(pin, pols), \
                (uint8_t*)result);
    }

    return ret_val;
}

zos_result_t io_expander_pol_set(const io_expander_device_t *dev, const io_expander_pin_t *pin,const io_expander_pol_t polarity)
{
    zos_result_t ret_val = ZOS_INVALID_ARG;

    if (dev != NULL && pin != NULL)
    {
        uint8_t pols[2];
#ifdef DRIVER_TCA6416
        if (tca6416_get_polarities(dev->device, pols) != ZOS_SUCCESS)
        {
            ret_val = ZOS_ERROR;
        }
        else
        {
            mask_values(pols, (uint8_t*)&polarity, pin);
            ret_val = tca6416_set_polarities(dev->device, pols);
        }
#endif
    }

    return ret_val;
}

/******************************************************
 *          Internal Function Definitions
 ******************************************************/

#ifdef DEBUG
/* Returns ZOS_TRUE if dev and data arguments are valid */
static zos_bool_t args_valid(const io_expander_device_t* dev, const void* data)
{
    return (zos_bool_t)(dev != NULL && data != NULL);
}
#endif

/* Calcs bitmask for pin set functions based on a pins bank and bitmask */
#if 0
static uint16_t calc_bitmask(const io_expander_pin_t *pin)
{
    uint16_t bitmask;

    if (pin->bank_id == 0U)
    {
        bitmask = pin->bitmask;
    }
    else
    {
        bitmask = ((uint16_t)(pin->bitmask)) << 8U;
    }

    return bitmask;
}
#endif

/* Apply bitmask to values from register */
static void mask_values(uint8_t *reg_val, const uint8_t *val, const io_expander_pin_t* pin)
{
    uint8_t* value_to_mask = get_bank_value(pin, reg_val);

    //set or clear pin value using bitmask based on given value
    if (*val == 0U)
    {
        *value_to_mask &= (uint8_t)~pin->bitmask;
    }
    else
    {
        *value_to_mask |= pin->bitmask;
    }
}

/* Get pointer to byte from register value according to bank of given pin */
static uint8_t* get_bank_value(const io_expander_pin_t* pin, uint8_t* values)
{
    uint8_t* bank_value;

    if (pin->bank_id == 0U)
    {
        bank_value = &values[0];
    }
    else
    {
        bank_value = &values[1];
    }

    return bank_value;
}

/* Set result value based on pin bitmask and bank value */
static void set_pin_result(uint8_t bitmask, uint8_t bank_value, uint8_t* result)
{
    uint8_t pin_value = bank_value & bitmask;

    if (pin_value > 0U)
    {
        *result = (uint8_t)1U;
    }
    else
    {
        *result = (uint8_t)0U;
    }
}
