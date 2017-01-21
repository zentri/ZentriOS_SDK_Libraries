/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "arducam_drivers.h"


extern const adrucam_driver_t* ov2640_get_driver(void);



static zos_spi_device_t *spi_device;
static zos_i2c_device_t *i2c_device;



/*************************************************************************************************/
zos_result_t adrucam_get_driver(arducam_type_t type, const adrucam_driver_t **driver_ptr)
{
    switch(type)
    {
#ifdef ARDUCAM_OV2640_ENABLED
    case ARDUCAM_TYPE_OV2640:
        *driver_ptr = ov2640_get_driver();
        break;
#endif
    default:
        return ZOS_NOT_FOUND;
    }

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t adrucam_driver_init(zos_spi_device_t *spi, zos_i2c_device_t *i2c, adrucam_driver_t *driver, const arducam_driver_config_t *config)
{
    spi_device = spi;
    i2c_device = i2c;

    return driver->callback.init(config);
}

/*************************************************************************************************/
uint8_t adrucam_driver_i2c_write_reg(uint16_t addr, uint8_t data)
{
    return (zn_i2c_master_write_reg8(i2c_device, addr, data) == ZOS_SUCCESS) ? 0 : 0xFF;
}

/*************************************************************************************************/
uint8_t adrucam_driver_i2c_read_reg(uint16_t addr, uint8_t *val)
{
    return (zn_i2c_master_read_reg8(i2c_device, addr, val) == ZOS_SUCCESS) ? *val : 0xFF;
}

/*************************************************************************************************/
zos_result_t adrucam_driver_i2c_write_regs(const reg_addr_value_t *regs, const reg_addr_value_t *action_list, uint8_t action_list_len)
{
    for(const reg_addr_value_t *addr_value = regs;; ++addr_value)
    {
        if(addr_value->address == REG_ADDR_ACTION)
        {
            if(addr_value->value == REG_ACTION_TERMINATE)
            {
                return ZOS_SUCCESS;
            }
            else if(action_list == NULL)
            {
                return ZOS_NULL_PTR;
            }
            else if(addr_value->value >= action_list_len)
            {
                return ZOS_BUFFER_OVERFLOW;
            }
            else
            {
                reg_addr_value_t *action = &action_list[addr_value->value];
                ZOS_VERIFY(adrucam_driver_i2c_write_reg(action->address, action->value));
            }
        }
        else
        {
            ZOS_VERIFY(adrucam_driver_i2c_write_reg(addr_value->address, addr_value->value));
        }
    }

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t adrucam_driver_i2c_write_reg_list(const uint8_t *list, uint16_t length)
{
    for(; length > 0; --length, ++list)
    {
        ZOS_VERIFY(adrucam_driver_i2c_write_reg(*list));
    }
    return ZOS_SUCCESS;
}
