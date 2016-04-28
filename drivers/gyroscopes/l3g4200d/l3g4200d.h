#ifndef __L3G4200D_GYRO_DRIVER__H
#define __L3G4200D_GYRO_DRIVER__H

#include <stdint.h>
#include "zos.h"
#include "sensor.h"
#include <math.h>
/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef struct {
  int16_t L3G4200D_GYRO_AXIS_X;
  int16_t L3G4200D_GYRO_AXIS_Y;
  int16_t L3G4200D_GYRO_AXIS_Z;
} L3G4200D_AxesRaw_t;

typedef enum {
  L3G4200D_GYRO_FULLSCALE_250             =               0x00,
  L3G4200D_GYRO_FULLSCALE_500             =               0x01,
  L3G4200D_GYRO_FULLSCALE_1000            =               0x02,
  L3G4200D_GYRO_FULLSCALE_2000            =               0x03
} L3G4200D_Fullscale_t;

/*  Register Addresses
*/
#define PMOD_GYRO_CTRL_REG1			0x20
#define PMOD_GYRO_CTRL_REG2			0x21
#define PMOD_GYRO_CTRL_REG3			0x22
#define PMOD_GYRO_CTRL_REG4			0x23
#define PMOD_GYRO_CTRL_REG5			0x24
#define PMOD_GYRO_OUT_TEMP			0x26
#define PMOD_GYRO_STATUS_REG		0x27
#define PMOD_GYRO_OUT_X_L			0x28
#define PMOD_GYRO_OUT_X_H			0x29
#define PMOD_GYRO_OUT_Y_L			0x2A
#define PMOD_GYRO_OUT_Y_H			0x2B
#define PMOD_GYRO_OUT_Z_L			0x2C
#define PMOD_GYRO_OUT_Z_H			0x2D

/*	CTRL_REG1
*/
#define PMOD_GYRO_REG1_DR1			0x80
#define PMOD_GYRO_REG1_DR0			0x40
#define PMOD_GYRO_REG1_BW1			0x20
#define PMOD_GYRO_REG1_BW0			0x10
#define PMOD_GYRO_REG1_PD			0x08
#define PMOD_GYRO_REG1_ZEN			0x04
#define PMOD_GYRO_REG1_YEN			0x02
#define PMOD_GYRO_REG1_XEN			0x01

/*	CTRL_REG3
*/
#define PMOD_GYRO_REG3_I1_INT1		0x80
#define PMOD_GYRO_REG3_I1_BOOT		0x40
#define PMOD_GYRO_REG3_H_LACTIVE	0x20
#define PMOD_GYRO_REG3_PP_OD		0x10
#define PMOD_GYRO_REG3_I2_DRDY		0x08
#define PMOD_GYRO_REG3_I2_WTM		0x04
#define PMOD_GYRO_REG3_I2_ORUN		0x02
#define PMOD_GYRO_REG3_I2_EMPTY		0x01


#define L3G4200D_SLAVE_ADDRESS  0x69

#define L3G4200D_DATA_RDY       0x08

zos_result_t L3G4200D_Init();
zos_result_t L3G4200D_ReadTemperature(uint8_t *temperature);
zos_result_t L3G4200D_GetAxesRaw(L3G4200D_AxesRaw_t* buff);
zos_result_t L3G4200D_GetStatusReg(uint8_t* val);
zos_result_t L3G4200D_SetFullScale(L3G4200D_Fullscale_t fs);
#endif //__L3G4200D_GYRO_DRIVER__H
