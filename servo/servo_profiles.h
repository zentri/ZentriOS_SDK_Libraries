/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#pragma once



#define SERVO_DUTY(pulse_length, frequency_hz) ((pulse_length*100) / (SERVO_DUTY_PADDING/frequency_hz))


// http://www.micropik.com/PDF/SG90Servo.pdf
#define SERVO_PROFILE_SG90 &(servo_profile_t) \
{                                           \
    .frequency = 50,                        \
    .duty_min = SERVO_DUTY(1000, 50),       \
    .duty_max = SERVO_DUTY(2000, 50),       \
    .duty_default = SERVO_DUTY(1500, 50),    \
}
