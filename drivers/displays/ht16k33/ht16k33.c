/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "ht16k33.h"


#define HT16K33_DEBUG_ENABLE
#ifdef HT16K33_DEBUG_ENABLE
#define HT16K33_DEBUG ZOS_LOG
#else
#define HT16K33_DEBUG(m, ...)
#endif


#define HT16K33_SLAVE_ADDRESS               0x70
#define HT16K33_REGISTER_DISPLAY_SETUP      0x80
#define HT16K33_REGISTER_SYSTEM_SETUP       0x20
#define HT16K33_REGISTER_DIMMING            0xE0
#define HT16K33_REGISTER_DISPLAY_DATA       0x00




/*************************************************************************************************/
zos_result_t ht16k33_init(zos_i2c_t i2c_port, zos_i2c_device_t *device)
{
    zos_result_t result;
    const uint8_t setup_reg = HT16K33_REGISTER_SYSTEM_SETUP | 1; // setup reg address + 'S' bit set

    device->port = i2c_port;
    device->retries = 3;
    device->address = HT16K33_SLAVE_ADDRESS;
    device->speed = I2C_CLOCK_STANDARD_SPEED;

    // turn the oscillator on
    if(ZOS_FAILED(result, write_ht16k33(device, setup_reg)))
    {
        HT16K33_DEBUG("Failed to write reg");
    }
    // turn blink off
    else if(ZOS_FAILED(result, ht16k33_set_blink_rate(device, HT16K33_BLINKRATE_OFF)))
    {
        HT16K33_DEBUG("Failed to disable blink");
    }
    // set max brightness
    else if(ZOS_FAILED(result, ht16k33_set_brightness(device, Ht16K33_BRIGHTNESS_MAX)))
    {
        HT16K33_DEBUG("Failed to set brightness");
    }

    return result;
}

/*************************************************************************************************/
void ht16k33_deinit(const zos_i2c_device_t *device)
{
}

/*************************************************************************************************/
zos_result_t ht16k33_set_brightness(const zos_i2c_device_t *device, uint8_t brightness)
{
    if (brightness > Ht16K33_BRIGHTNESS_MAX)
    {
        brightness = Ht16K33_BRIGHTNESS_MAX;
    }
    brightness = HT16K33_REGISTER_DIMMING | brightness;

    return write_ht16k33(device, brightness);
}

/*************************************************************************************************/
zos_result_t ht16k33_set_blink_rate(const zos_i2c_device_t *device, uint8_t blinkRate)
{
    if (blinkRate > HT16K33_BLINKRATE_HALFHZ)
    {
        blinkRate = HT16K33_BLINKRATE_OFF;
    }
    blinkRate = HT16K33_REGISTER_DISPLAY_SETUP | 0x01 | (blinkRate << 1);

    return write_ht16k33(device, blinkRate);
}

/*************************************************************************************************/
zos_result_t ht16k33_write_display(const zos_i2c_device_t *device, uint16_t *buffer)
{
    uint16_t *ptr = buffer;
    for(int i = 0; i < 8; ++i, ++ptr)
    {
         const uint16_t v = *ptr;
        *ptr = ((v & 0xfe) >> 1) | ((v & 0x01) << 7);
    }

    // Send 8x16-bits of data
    return zn_i2c_master_write_reg(device, HT16K33_REGISTER_DISPLAY_DATA, (const uint8_t*)buffer, 16);
}

/*************************************************************************************************/
zos_result_t ht16k33_clear_display(const zos_i2c_device_t *device)
{
    uint16_t dummy[8];
    memset(dummy, 0, sizeof(dummy));
    return ht16k33_write_display(device, dummy);
}

/*************************************************************************************************/
static zos_result_t write_ht16k33(const zos_i2c_device_t *device, uint8_t value)
{
    const zos_i2c_message_t msg =
    {
           .buffer = &value,
           .length = 1,
           .flags = I2C_MSG_WRITE
    };

    return zn_i2c_transfer(device, &msg, 1);
}
