/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "arducam_drivers.h"
#include "arduchip.h"



extern const arducam_driver_t* ov2640_get_driver(void);



static zos_spi_device_t *spi_device;
static zos_i2c_device_t *i2c_device;



/*************************************************************************************************/
zos_result_t arducam_get_driver(arducam_type_t type, const arducam_driver_t **driver_ptr)
{
    switch(type)
    {
#ifdef DRIVER_CAMERA_OV2640
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
zos_result_t arducam_driver_init(zos_spi_device_t *spi, zos_i2c_device_t *i2c, const arducam_driver_t *driver, const arducam_config_t *config)
{
    zos_result_t result;

    spi_device = spi;
    i2c_device = i2c;

    spi_device->port = config->spi.port;
    spi_device->chip_select = config->spi.cs;
    spi_device->flags = driver->spi.flags;
    spi_device->speed = driver->spi.rate;
    i2c_device->port = config->i2c_port;
    i2c_device->address = driver->i2c.address;
    i2c_device->read_timeout = driver->i2c.read_timeout;
    i2c_device->retries = driver->i2c.retries;
    i2c_device->speed = driver->i2c.rate;

    if(ZOS_FAILED(result, zn_i2c_init(i2c_device)))
    {
        return result;
    }

    zn_gpio_deinit(spi_device->chip_select);
    zn_gpio_init(spi_device->chip_select, GPIO_OUTPUT_PUSHPULL, ZOS_TRUE);

    zn_rtos_delay_milliseconds(100);

    return driver->callback.init(&config->driver_config);
}

/*************************************************************************************************/
uint8_t arducam_driver_i2c_write_reg(uint8_t addr, uint8_t data)
{
    return (zn_i2c_master_write_reg8(i2c_device, addr, data) == ZOS_SUCCESS) ? 0 : 0xFF;
}

/*************************************************************************************************/
uint8_t arducam_driver_i2c_read_reg(uint8_t addr, uint8_t *val)
{
    return (zn_i2c_master_read_reg8(i2c_device, addr, val) == ZOS_SUCCESS) ? *val : 0xFF;
}

/*************************************************************************************************/
zos_result_t arducam_driver_i2c_write_regs(const reg_addr_value_t *regs, const reg_addr_value_t *action_list, uint8_t action_list_len)
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
                const reg_addr_value_t *action = &action_list[addr_value->value];
                ZOS_VERIFY(arducam_driver_i2c_write_reg(action->address, action->value));
            }
        }
        else
        {
            ZOS_VERIFY(arducam_driver_i2c_write_reg(addr_value->address, addr_value->value));
        }
    }

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t arducam_driver_spi_write_reg(uint8_t addr, uint8_t data)
{
    addr |= ARDUCHIP_RW_FLAG;

    const zos_spi_message_t messages[2] =
    {
            { .tx_buffer = &addr, .length = 1 },
            { .tx_buffer = &data, .length = 1 },
    };

    ZOS_VERIFY(zn_spi_transfer(spi_device, messages, 2));

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t arducam_driver_spi_read_reg(uint8_t addr, uint8_t *data_ptr)
{
    zos_result_t result;

    addr &= ~ARDUCHIP_RW_FLAG;

    const zos_spi_message_t messages[2] =
    {
            { .tx_buffer = &addr, .length = 1 },
            { .rx_buffer = data_ptr, .length = 1 },
    };

    if(!ZOS_FAILED(result, zn_spi_transfer(spi_device, messages, 2)))
    {
        //ZOS_LOG("SPI read reg: 0x%02X = 0x%02X", addr, *data_ptr);
    }

    return result;
}

/*************************************************************************************************/
zos_result_t arducam_driver_spi_clear_bit(uint8_t addr, uint8_t bits)
{
    uint8_t reg_value;

    ZOS_VERIFY(arducam_driver_spi_read_reg(addr, &reg_value));

    reg_value &= ~bits;

    ZOS_VERIFY(arducam_driver_spi_write_reg(addr, reg_value));

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t arducam_driver_spi_set_bit(uint8_t addr, uint8_t bits)
{
    uint8_t reg_value;

    ZOS_VERIFY(arducam_driver_spi_read_reg(addr, &reg_value));

    reg_value |= bits;

    ZOS_VERIFY(arducam_driver_spi_write_reg(addr, reg_value));


    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t arducam_driver_spi_burst_read_start(void)
{
    const uint8_t burst_read_command = BURST_FIFO_READ;
    const uint8_t dummy_byte = 0;

    spi_device->flags |= SPI_FLAG_KEEP_ASSERTED;

    const zos_spi_message_t messages[2] =
    {
            { .tx_buffer = &burst_read_command, .length = 1 },
            { .tx_buffer = &dummy_byte, .length = 1 },
    };

    return zn_spi_transfer(spi_device, messages, 2);
}

/*************************************************************************************************/
zos_result_t arducam_driver_spi_burst_read(uint8_t *buffer, uint16_t length)
{
    const zos_spi_message_t messages[1] =
    {
            { .rx_buffer = buffer, .length = length }
    };

    return zn_spi_transfer(spi_device, messages, 1);
}

/*************************************************************************************************/
zos_result_t arducam_driver_spi_burst_read_stop(void)
{
    spi_device->flags &= ~SPI_FLAG_KEEP_ASSERTED;
    zn_spi_master_deassert(spi_device);

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t arducam_driver_get_fifo_size(uint32_t *size_ptr)
{
    uint32_t size;
    uint8_t size_part;

    *size_ptr = 0;

    ZOS_VERIFY(arducam_driver_spi_read_reg(FIFO_SIZE3, &size_part));
    size = size_part;
    size <<= 8;

    ZOS_VERIFY(arducam_driver_spi_read_reg(FIFO_SIZE2, &size_part));
    size |= (uint32_t)size_part;
    size <<= 8;

    ZOS_VERIFY(arducam_driver_spi_read_reg(FIFO_SIZE1, &size_part));
    size |= (uint32_t)size_part;

    *size_ptr = size;

    return ZOS_SUCCESS;
}
