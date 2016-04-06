/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#ifndef __MPU9250_GYRO_DRIVER__H
#define __MPU9250_GYRO_DRIVER__H

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <stdint.h>
#include "zos_types.h"


/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef struct {
  int16_t MPU9250_GYRO_AXIS_X;
  int16_t MPU9250_GYRO_AXIS_Y;
  int16_t MPU9250_GYRO_AXIS_Z;
} MPU9250_GyroAxesRaw_t;

typedef enum {
  MPU9250_GYRO_FULLSCALE_250             =               0x00,
  MPU9250_GYRO_FULLSCALE_500             =               0x01,
  MPU9250_GYRO_FULLSCALE_1000            =               0x02,
  MPU9250_GYRO_FULLSCALE_2000            =               0x03
} MPU9250_GyroFullscale_t;

typedef enum {
  MPU9250_RESET_DISABLED                 =               0,
  MPU9250_RESET_ENABLED                  =               1
} MPU9250_Reset_t;

typedef enum {
  MPU9250_SLEEP_DISABLED                 =               0,
  MPU9250_SLEEP_ENABLED                  =               1
} MPU9250_Sleep_t;

typedef enum {
  MPU9250_CYCLE_DISABLED                 =               0,
  MPU9250_CYCLE_ENABLED                  =               1
} MPU9250_Cycle_t;

/******************************************************************************/
/*  Register Definitions                                                      */
/******************************************************************************/
// Gyroscope Configuration Register (fullscale select)
#define MPU9250_GYRO_CFG                                0x1B
#define MPU9250_FSS                                      (3)

// Power Management Register
#define MPU9250_PWR_MGMT_1                               0x6B
#define MPU9250_RESET                                    (7)
#define MPU9250_SLEEP                                    (6)
#define MPU9250_CYCLE                                    (5)

// Power Management Register 2 (enable/disable axes)
#define MPU9250_PWR_MGMT_2                               0x6C
#define MPU9250_ENABLE_XG                                (2)
#define MPU9250_ENABLE_YG                                (1)
#define MPU9250_ENABLE_ZG                                (0)

// Output Register
#define MPU9250_GYRO_XOUT_H                             0x43
#define MPU9250_GYRO_XOUT_L                             0x44
#define MPU9250_GYRO_YOUT_H                             0x45
#define MPU9250_GYRO_YOUT_L                             0x46
#define MPU9250_GYRO_ZOUT_H                             0x47
#define MPU9250_GYRO_ZOUT_L                             0x48

// Status Register
#define MPU9250_INTERRUPT_STATUS                         0x3A
#define MPU9250_RAW_DATA_RDY_INT                         0x01

// Sensor I2C Slave Address
#define MPU9250_SLAVE_ADDRESS                            0x68

/******************************************************
 *          External Function Definitions
 ******************************************************/

zos_result_t MPU9250_GyroSetFullScale(MPU9250_GyroFullscale_t fs);
zos_result_t MPU9250_GyroSetAxis(uint8_t enableX, uint8_t enableY, uint8_t enableZ);
zos_result_t MPU9250_GyroGetStatusReg(uint8_t* val);
zos_result_t MPU9250_GyroGetAxesRaw(MPU9250_GyroAxesRaw_t* buff);
zos_result_t MPU9250_GyroSetPowerMgmt(MPU9250_Reset_t reset, MPU9250_Sleep_t sleep, MPU9250_Cycle_t cycle);
zos_result_t MPU9250_GyroGetFullScale(MPU9250_GyroFullscale_t* fs);

#endif /* __MPU9250_GYRO_DRIVER__H */

