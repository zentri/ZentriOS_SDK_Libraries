/**************************************************************************//**
*   @file   AD7156.h
*   @brief  AD7156 Driver Header file.
*   @author ATofan (alexandru.tofan@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/

#ifndef AD7156_H_
#define AD7156_H_

#include "zos.h"
#include "sensor.h"
#include <math.h>

typedef struct
{
	int16_t ch1_capacitance;
	int16_t ch2_capacitance;
	float adjusted_ch1_cap;
	float adjusted_ch2_cap;
} AD7156_data_t;

/******************************************************************************/
// AD7156 Registers
/******************************************************************************/
/*************************** AD7156 Definitions *******************************/
/******************************************************************************/
/*!< AD7156 slave address */
#define AD7156_I2C_ADDR                 0x48

/*!< AD7156 registers */
#define AD7156_REG_STATUS               0x00
#define AD7156_REG_CH1_DATA_H           0x01
#define AD7156_REG_CH1_DATA_L           0x02
#define AD7156_REG_CH2_DATA_H           0x03
#define AD7156_REG_CH2_DATA_L           0x04
#define AD7156_REG_CH1_AVG_H            0x05
#define AD7156_REG_CH1_AVG_L            0x06
#define AD7156_REG_CH2_AVG_H            0x07
#define AD7156_REG_CH2_AVG_L            0x08
#define AD7156_REG_CH1_SENS_THRSH_H     0x09
#define AD7156_REG_CH1_TMO_THRSH_L      0x0A
#define AD7156_REG_CH1_SETUP            0x0B
#define AD7156_REG_CH2_SENS_THRSH_H     0x0C
#define AD7156_REG_CH2_TMO_THRSH_L      0x0D
#define AD7156_REG_CH2_SETUP            0x0E
#define AD7156_REG_CONFIG               0x0F
#define AD7156_REG_PWR_DWN_TMR          0x10
#define AD7156_REG_CH1_CAPDAC           0x11
#define AD7156_REG_CH2_CAPDAC           0x12
#define AD7156_REG_SERIAL_N3            0x13
#define AD7156_REG_SERIAL_N2            0x14
#define AD7156_REG_SERIAL_N1            0x15
#define AD7156_REG_SERIAL_N0            0x16
#define AD7156_REG_CHIP_ID              0x17

/*!< AD7156_REG_STATUS definition */
#define AD7156_STATUS_PWR_DWN           (1 << 7)
#define AD7156_STATUS_DAC_STEP2         (1 << 6)
#define AD7156_STATUS_OUT2              (1 << 5)
#define AD7156_STATUS_DAC_STEP1         (1 << 4)
#define AD7156_STATUS_OUT1              (1 << 3)
#define AD7156_STATUS_C1_C2             (1 << 2)
#define AD7156_STATUS_RDY2              (1 << 1)
#define AD7156_STATUS_RDY1              (1 << 0)

/*!< AD7156_REG_CH1_SETUP definition */
#define AD7156_CH1_SETUP_RANGE(x)       (((x) & 0x3) << 6)
#define AD7156_CH1_SETUP_HYST1          (1 << 4)
#define AD7156_CH1_SETUP_THR1(x)        ((x) & 0xF)

/*!< AD7156_REG_CH2_SETUP definition */
#define AD7156_CH2_SETUP_RANGE(x)       (((x) & 0x3) << 6)
#define AD7156_CH2_SETUP_HYST2          (1 << 4)
#define AD7156_CH2_SETUP_THR2(x)        ((x) & 0xF)

/*!< AD7156_CH1_SETUP_RANGE(x) and AD7156_CH2_SETUP_RANGE(x) options */
#define AD7156_CDC_RANGE_2_PF           0
#define AD7156_CDC_RANGE_0_5_PF         1
#define AD7156_CDC_RANGE_1_PF           2
#define AD7156_CDC_RANGE_4_PF           3

/*!< AD7156_REG_CONFIG definition */
#define AD7156_CONFIG_THR_FIXED         (1 << 7)
#define AD7156_CONFIG_THR_MD(x)         (((x) & 0x3) << 5)
#define AD7156_CONFIG_EN_CH1            (1 << 4)
#define AD7156_CONFIG_EN_CH2            (1 << 3)
#define AD7156_CONFIG_MD(x)             ((x) & 0x7)

/*!< AD7156_CONFIG_THR_FIXED options */
#define AD7156_ADAPTIVE_THRESHOLD       0
#define AD7156_FIXED_THRESHOLD          1

/*!< AD7156_CONFIG_THR_MD(x) options */
#define AD7156_THR_MODE_NEGATIVE        0
#define AD7156_THR_MODE_POSITIVE        1
#define AD7156_THR_MODE_IN_WINDOW       2
#define AD7156_THR_MODE_OU_WINDOW       3

/*!< AD7156_CONFIG_MD(x) options */
#define AD7156_CONV_MODE_IDLE            0
#define AD7156_CONV_MODE_CONT_CONV       1
#define AD7156_CONV_MODE_SINGLE_CONV     2
#define AD7156_CONV_MODE_PWR_DWN         3

/*!< AD7156_REG_PWR_DWN_TMR definition */
#define AD7156_PWR_DWN_TMR_TIMEOUT(x)   (((x) & 0x3F) | (1 << 6))

/*!< AD7156_REG_CH1_CAPDAC */
#define AD7156_CH1_CAPDAC_DAC_EN1       (1 << 7)
#define AD7156_CH1_CAPDAC_DAC_AUTO1     (1 << 6)
#define AD7156_CH1_CAPDAC_DAC_VAL1(x)   ((x) & 0x3F)

/*!< AD7156_REG_CH2_CAPDAC */
#define AD7156_CH2_CAPDAC_DAC_EN2       (1 << 7)
#define AD7156_CH2_CAPDAC_DAC_AUTO2     (1 << 6)
#define AD7156_CH2_CAPDAC_DAC_VAL2(x)   ((x) & 0x3F)

/*!< Chip identification code */
#define AD7156_DEFAULT_ID               0x88

/*!< AD7156 Reset Command */
#define AD7156_RESET_CMD                0xBF

/*!< AD7156 Channels */
#define AD7156_CHANNEL1                 1
#define AD7156_CHANNEL2                 2

#define AD7156_RngH					7
#define AD7156_RngL					6
#define AD7156_nHyst				4
#define AD7156_ThrSettling			0
// Configuration Register 0x0F
#define AD7156_ThrFixed				7
#define AD7156_ThrMD1				6
#define AD7156_ThrMD0				5
#define AD7156_EnCh1				4
#define AD7156_EnCh2				3
#define AD7156_MD2					2
#define AD7156_MD1					1
#define AD7156_MD0					0
/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*!< Initializes the communication peripheral and checks if the device is
    present. */
zos_result_t AD7156_Init(void);

/*!< Resets the device. */
zos_result_t AD7156_Reset(void);

/*!< Sets the converter mode of operation. */
zos_result_t AD7156_SetPowerMode(uint8_t pwrMode);
/*!< Enables or disables conversion on the selected channel. */
zos_result_t AD7156_ChannelState(uint8_t channel, uint8_t enableConv);
/*!< Sets the input range of the specified channel. */
zos_result_t AD7156_SetRange(unsigned channel, uint8_t range);
/*!< Reads the range bits from the device and returns the range in pF. */
zos_result_t AD7156_GetRange(unsigned channel, float *range);
/*!< Selects the threshold mode of operation. */
zos_result_t AD7156_SetThresholdMode(uint8_t thrMode, uint8_t thrFixed);
/*!< Writes to the threshold register when threshold fixed mode is enabled. */
zos_result_t AD7156_SetThreshold(uint8_t channel, float pFthr);
/*!< Writes a value(pF) to the sensitivity register. This functions
    should be used when adaptive threshold mode is selected. */
zos_result_t AD7156_SetSensitivity(uint8_t channel, float pFsensitivity);
/*!< Reads a 12-bit sample from the selected channel */
zos_result_t AD7156_SetTimeout(uint8_t channel, uint8_t AprRecTimeout);

zos_result_t AD7156_ReadChannelData(uint8_t channel, uint16_t *chResult);
/*!< Waits for a finished CDC conversion and reads a 12-bit sample from
    the selected channel. */
zos_result_t AD7156_WaitReadChannelData(uint8_t channel, uint16_t *chResult);
/*!< Reads a sample the selected channel and converts the data to
   picofarads(pF). */
zos_result_t AD7156_ReadChannelCapacitance(uint8_t channel, float *pFdata);
/*!< Waits for a finished CDC conversion the selected channel, reads a
    sample and converts the data to picofarads(pF). */
zos_result_t AD7156_WaitReadChannelCapacitance(uint8_t channel, float *pFdata);

zos_result_t AD7156_GetStatusReg(uint8_t* val);
zos_result_t AD7156_EnableHysteresis(uint8_t channel, zos_bool_t HystEn);

#endif /* AD7156_H_ */

