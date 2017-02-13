/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once



#include "zos.h"

#include "servo_profiles.h"


#define SERVO_DUTY_PADDING 1000

#define SERVO_FULL_LEFT_POSITION    0
#define SERVO_MIDDLE_POSITION       5000
#define SERVO_FULL_RIGHT_POSITION   10000


typedef struct
{
    uint32_t frequency;     // PWM frequency
    uint32_t duty_min;      // Duty cycle (in percent) when servo is at full left position or slowest rate, padded with SERVO_DUTY_PADDING
    uint32_t duty_max;      // Duty cycle (in percent) when servo is at full right position or fastest rate, padded with SERVO_DUTY_PADDING
    uint32_t duty_default;  // Default duty cycle (in percent) when servo is at rest position, padded with SERVO_DUTY_PADDING
} servo_profile_t;


/**
 * Initialize server PWM signal with specific profile
 *
 * @param gpio The GPIO with PWM functionality
 * @param servo_profile The profile of the servo, see @ref servo_profile_t
 */
zos_result_t servo_init(zos_gpio_t gpio, const servo_profile_t *servo_profile);

/**
 * De-initialize a servo and PWM signal
 */
zos_result_t servo_deinit(zos_gpio_t gpio);

/**
 * Set the position of the servo.
 *
 * This is intended for non-fully rotational servos.
 *
 * The `position` argument has the following meanings:
 * 0     - fully left
 * 5000  - middle or 'zero' position
 * 10000 - fully right
 *
 * @param gpio The GPIO with PWM functionality
 * @param position Position to set servo
 */
zos_result_t servo_set_position(zos_gpio_t gpio, uint32_t position);

/**
 * Get the servo's current position
 * @param gpio The GPIO with PWM functionality
 * @param position_ptr
 * @return
 */
zos_result_t servo_get_position(zos_gpio_t gpio, uint32_t *position_ptr);

/**
 * Set the rotation rate of the servo
 *
 * This is intended for fully rotational servos.
 *
 * The `speed` argument has the following meanings:
 * 0     - stopped
 * 5000  - half rotation speed
 * 10000 - full rotation speed
 *
 * @param gpio The GPIO with PWM functionality
 * @param speed Rate to spin servo
 */
zos_result_t servo_set_speed(zos_gpio_t gpio, uint32_t speed);

/**
 *
 * @param gpio
 * @param speed_ptr
 * @return
 */
zos_result_t servo_get_speed(zos_gpio_t gpio, uint32_t *speed_ptr);

