/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#ifdef OM13086SCCK_BUILD


#include "om13082.h"


#define JOYSTICK_DEBOUNCE 75 // ms
#define JOYSTICK_DEBOUNCE_RESOLUTION 10

#define PCA9535_SLAVE_ADDRESS 0x20
#define PCA9535_REG_INPUT_0 0
#define PCA9535_REG_INPUT_1 1
#define PCA9535_REG_OUTPUT_0 2
#define PCA9535_REG_OUTPUT_1 3
#define PCA9535_REG_POLARITY_0 4
#define PCA9535_REG_POLARITY_1 5
#define PCA9535_REG_CONFIG_0 6
#define PCA9535_REG_CONFIG_1 7

#define LED_REG PCA9535_REG_OUTPUT_1
#define LED_CONFIG_REG PCA9535_REG_CONFIG_1
#define LED_MASK 0xF0
#define LED1_MASK 0x10
#define LED2_MASK 0x20
#define LED3_MASK 0x40
#define LED4_MASK 0x80

#define JOYSTICK_REG PCA9535_REG_INPUT_0
#define JOYSTICK_COFNIG_REG PCA9535_REG_CONFIG_0
#define JOYSTICK_MASK 0x1F
#define JOYSTICK_DOWN_MASK 0x01
#define JOYSTICK_SELECT_MASK 0x02
#define JOYSTICK_LEFT_MASK 0x04
#define JOYSTICK_RIGHT_MASK 0x08
#define JOYSTICK_UP_MASK 0x10


#define PCA9535_IRQ_GPIO ZOS_GPIO_0




static zos_result_t pca9535_read_reg(uint8_t reg, uint8_t *value_ptr);
static zos_result_t pca9535_write_reg(uint8_t reg, uint8_t value);
static void om13082_joystick_gpio_irq(void *arg);
static void joystick_debounce_timer_handler(void *arg);


static const uint8_t const led_mask_mapping[] =
{
        [LED1] = LED1_MASK,
        [LED2] = LED2_MASK,
        [LED3] = LED3_MASK,
        [LED4] = LED4_MASK,
};

static const zos_i2c_device_t i2c_pca9535 =
{
     .port = ZOS_I2C_1,
    .address = (uint16_t)PCA9535_SLAVE_ADDRESS,
    .speed = I2C_CLOCK_STANDARD_SPEED,
    .retries = (uint16_t)1U,
    .read_timeout = 40
};

static struct
{
    zos_event_handler_t joystick_gpio_irq_event_handler;
    zos_timer_t debounce_timer;
    uint32_t debounce_stamp;
} om13082_context;



/*************************************************************************************************/
zos_result_t om13082_init(void)
{
    zos_result_t result;

    // Start LEDs OFF (note: led pins are active low)
    if(ZOS_FAILED(result, pca9535_write_reg(LED_REG, LED_MASK)))
    {
    }
    // Set LED pins as output (setting bit to 0 make pin an output)
    else if(ZOS_FAILED(result, pca9535_write_reg(LED_CONFIG_REG, (uint8_t)~LED_MASK)))
    {
    }
    else
    {
        zn_rtos_timer_init(&om13082_context.debounce_timer, JOYSTICK_DEBOUNCE_RESOLUTION, joystick_debounce_timer_handler, NULL);
    }

    return result;
}

/*************************************************************************************************/
zos_result_t om13082_joystick_set_events_enabled(zos_event_handler_t event_handler, zos_bool_t enabled)
{
    zos_result_t result;

    if(enabled && om13082_context.joystick_gpio_irq_event_handler == NULL)
    {
        uint8_t value;

        zn_gpio_init(PCA9535_IRQ_GPIO, GPIO_INPUT_HIGHZ, ZOS_FALSE);
        om13082_context.joystick_gpio_irq_event_handler = event_handler;


        // clear the interrupts by reading the inputs
        if(ZOS_FAILED(result, pca9535_read_reg(PCA9535_REG_INPUT_0, &value)))
        {
        }
        else if(ZOS_FAILED(result, pca9535_read_reg(PCA9535_REG_INPUT_1, &value)))
        {
        }
        else if(ZOS_FAILED(result, zn_gpio_irq_enable(PCA9535_IRQ_GPIO, GPIO_IRQ_TRIGGER_FALLING_EDGE, om13082_joystick_gpio_irq, NULL)))
        {
        }
        else if(!zn_event_irq_events_enabled())
        {
            zn_event_enable_irq_events(8);
            om13082_context.debounce_stamp = 0;
        }
    }
    else
    {
        om13082_context.joystick_gpio_irq_event_handler = NULL;
        zn_gpio_irq_disable(PCA9535_IRQ_GPIO);
        zn_rtos_timer_stop(&om13082_context.debounce_timer);
        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
zos_bool_t om13082_joystick_get_value(joystick_direction_t direction)
{
    joystick_direction_t dir = om13082_joystick_get_direction();

    return (dir & direction) ? ZOS_TRUE : ZOS_FALSE;
}

/*************************************************************************************************/
joystick_direction_t om13082_joystick_get_direction(void)
{
    joystick_direction_t retval = 0xFF;

    if(pca9535_read_reg(JOYSTICK_REG, (uint8_t*)&retval) == ZOS_SUCCESS)
    {
    }

    return ~retval & JOYSTICK_MASK;
}

/*************************************************************************************************/
const char* om13082_joystick_get_direction_str(joystick_direction_t direction)
{
    return (direction & JOYSTICK_UP)       ? "up" :
           (direction & JOYSTICK_DOWN)     ? "down" :
           (direction & JOYSTICK_LEFT)     ? "left" :
           (direction & JOYSTICK_SELECT)   ? "select" :
           (direction & JOYSTICK_RIGHT)    ? "right" : "unknown";
}

/*************************************************************************************************/
zos_result_t om13082_led_set_value(led_t led, zos_bool_t enabled)
{
    zos_result_t result;
    uint8_t value;

    if(!ZOS_FAILED(result, pca9535_read_reg(LED_REG, &value)))
    {
        const uint8_t led_val = (~value) & led_mask_mapping[led];

        if(enabled && !led_val)
        {
            result = pca9535_write_reg(LED_REG, value & ~led_mask_mapping[led]);
        }
        else if(!enabled && led_val)
        {
            result = pca9535_write_reg(LED_REG, value | led_mask_mapping[led]);
        }
    }

    return result;
}

/*************************************************************************************************/
zos_bool_t om13082_led_get_value(led_t led)
{
    uint8_t reg;
    zos_bool_t retval = ZOS_FALSE;

    if(pca9535_read_reg(LED_REG, &reg) == ZOS_SUCCESS)
    {
        reg = ~reg;
        retval = (reg & led_mask_mapping[led]) ? ZOS_TRUE : ZOS_FALSE;
    }
    return retval;
}



/*************************************************************************************************/
static zos_result_t pca9535_read_reg(uint8_t reg, uint8_t *value_ptr)
{
    return zn_i2c_master_read_reg8(&i2c_pca9535, reg, value_ptr);
}

/*************************************************************************************************/
static zos_result_t pca9535_write_reg(uint8_t reg, uint8_t value)
{
    return zn_i2c_master_write_reg8(&i2c_pca9535, reg, value);
}

/*************************************************************************************************/
static void om13082_joystick_gpio_irq(void *arg)
{
    om13082_context.debounce_stamp = zn_rtos_get_time();
    if(!zn_rtos_timer_is_running(&om13082_context.debounce_timer))
    {
        zn_rtos_timer_start(&om13082_context.debounce_timer);
    }
}

/*************************************************************************************************/
static void joystick_debounce_timer_handler(void *arg)
{
    const uint32_t now = zn_rtos_get_time();
    if((now - om13082_context.debounce_stamp) > JOYSTICK_DEBOUNCE)
    {
        zn_rtos_timer_stop(&om13082_context.debounce_timer);
        zn_event_issue(om13082_context.joystick_gpio_irq_event_handler, NULL, EVENT_FLAGS1(FROM_IRQ));
    }
}

#endif
