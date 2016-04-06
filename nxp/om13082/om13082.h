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


typedef enum
{
    LED1  = 0,
    LED2  = 1,
    LED3  = 2,
    LED4  = 3
} led_t;



typedef enum
{
    JOYSTICK_NONE       = 0,
    JOYSTICK_DOWN       = 0x01,
    JOYSTICK_SELECT     = 0x02,
    JOYSTICK_LEFT       = 0x04,
    JOYSTICK_RIGHT      = 0x08,
    JOYSTICK_UP         = 0x10,
} joystick_direction_t;



zos_result_t om13082_init(void);

zos_result_t om13082_joystick_set_events_enabled(zos_event_handler_t event_handler, zos_bool_t enabled);

zos_bool_t om13082_joystick_get_value(joystick_direction_t direction);

joystick_direction_t om13082_joystick_get_direction(void);

const char* om13082_joystick_get_direction_str(joystick_direction_t direction);

zos_result_t om13082_led_set_value(led_t led, zos_bool_t enabled);

zos_bool_t om13082_led_get_value(led_t led);

