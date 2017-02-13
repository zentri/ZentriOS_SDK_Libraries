/*
 * servo.c
 *
 *  Created on: Jan 17, 2017
 *      Author: reed
 */

#include "servo.h"


#define MAX_RANGE (100 - 0)
#define POSITION_PADDING 100
#define SPEED_PADDING 100


typedef struct servo_context
{
    struct servo_context *next;
    zos_gpio_t gpio;    // this GPIO MUST be mapped to a PWM
    servo_profile_t profile;
    uint32_t current_duty;
} servo_context_t;


static servo_context_t *servo_list;


/*************************************************************************************************/
zos_result_t servo_init(zos_gpio_t gpio, const servo_profile_t *servo_profile)
{
    zos_result_t result;
    servo_context_t *context;

    zn_gpio_deinit(gpio);
    servo_deinit(gpio);

    if(ZOS_FAILED(result, zn_pwm_update(gpio, servo_profile->duty_default, servo_profile->frequency)))
    {
    }
    if(ZOS_FAILED(result, zn_malloc((uint8_t**)&context, sizeof(servo_context_t))))
    {
        zn_pwm_stop(gpio);
    }
    else
    {
        context->gpio = gpio;
        context->current_duty = servo_profile->duty_default;
        memcpy(&context->profile, servo_profile, sizeof(servo_profile_t));
        context->next = servo_list;
        servo_list = context;
    }

    return result;
}

/*************************************************************************************************/
zos_result_t servo_deinit(zos_gpio_t gpio)
{
    servo_context_t *prev = NULL;

    for(servo_context_t *context = servo_list; context != NULL; prev = context, context = context->next)
    {
        if(context->gpio == gpio)
        {
            if(prev != NULL)
            {
                prev->next = context->next;
            }
            else
            {
                servo_list = NULL;
            }

            zn_pwm_stop(gpio);
            free(context);

            return ZOS_SUCCESS;
        }
    }

    return ZOS_NOT_FOUND;
}

/*************************************************************************************************/
zos_result_t servo_set_position(zos_gpio_t gpio, uint32_t position)
{
    zos_result_t result;
    servo_context_t *context = get_servo_context(gpio);
    if(context == NULL)
    {
        return ZOS_NOT_FOUND;
    }

    const uint32_t duty = (context->profile.duty_min*POSITION_PADDING +
            ((position * (context->profile.duty_max - context->profile.duty_min)) / MAX_RANGE)) / SERVO_DUTY_PADDING;

    if(!ZOS_FAILED(result, zn_pwm_update_with_resolution(gpio, duty, context->profile.frequency)))
    {
        context->current_duty = duty;
    }

    return result;
}

/*************************************************************************************************/
zos_result_t servo_get_position(zos_gpio_t gpio, uint32_t *position_ptr)
{
    servo_context_t *context = get_servo_context(gpio);
    if(context == NULL)
    {
        return ZOS_NOT_FOUND;
    }
    else
    {
        *position_ptr = context->current_duty;

        return ZOS_SUCCESS;
    }
}

/*************************************************************************************************/
zos_result_t servo_set_speed(zos_gpio_t gpio, uint32_t speed)
{
    servo_context_t *context = get_servo_context(gpio);
    if(context == NULL)
    {
        return ZOS_NOT_FOUND;
    }

    if(speed == 0)
    {
        return zn_pwm_stop(gpio);
    }
    else
    {
        const uint32_t duty =  (context->profile.duty_min*SPEED_PADDING +
                ((speed * (context->profile.duty_max - context->profile.duty_min)) / MAX_RANGE)) / SERVO_DUTY_PADDING;

        return zn_pwm_update_with_resolution(gpio, duty, context->profile.frequency);
    }

}

/*************************************************************************************************/
zos_result_t servo_get_speed(zos_gpio_t gpio, uint32_t *speed_ptr)
{
    servo_context_t *context = get_servo_context(gpio);
    if(context == NULL)
    {
        return ZOS_NOT_FOUND;
    }
    else
    {
        *speed_ptr = context->current_duty;

        return ZOS_SUCCESS;
    }
}

/*************************************************************************************************/
static servo_context_t* get_servo_context(zos_gpio_t gpio)
{
    for(servo_context_t *context = servo_list; context != NULL; context = context->next)
    {
        if(context->gpio == gpio)
        {
            return context;
        }
    }

    return NULL;
}
