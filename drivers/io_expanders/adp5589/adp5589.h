/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#ifndef adp5589_DRIVER_H_
#define adp5589_DRIVER_H_

#include <stdint.h>
#include "zos.h"

#define ADP5589_ID			0x00
#define ADP5589_INT_STATUS	0x01
#define ADP5589_STATUS		0x02
#define ADP5589_FIFO_1		0x03
#define ADP5589_FIFO_2		0x04
#define ADP5589_FIFO_3		0x05
#define ADP5589_FIFO_4		0x06
#define ADP5589_FIFO		0x07
#define ADP5589_FIFO_6		0x08
#define ADP5589_FIFO_7		0x09
#define ADP5589_FIFO_8		0x0A
#define ADP5589_FIFO_9		0x0B
#define ADP5589_FIFO_10		0x0C
#define ADP5589_FIFO_11		0x0D
#define ADP5589_FIFO_12		0x0E
#define ADP5589_FIFO_13		0x0F
#define ADP5589_FIFO_14		0x10
#define ADP5589_FIFO_15		0x11
#define ADP5589_FIFO_16		0x12
#define ADP5589_GPI_INT_STAT_A		0x13
#define ADP5589_GPI_INT_STAT_B		0x14

/* ADP5589 Registers */
#define ADP5589_GPI_INT_STAT_C		0x15
#define ADP5589_GPI_STATUS_A		0x16
#define ADP5589_GPI_STATUS_B		0x17
#define ADP5589_GPI_STATUS_C		0x18
#define ADP5589_RPULL_CONFIG_A		0x19
#define ADP5589_RPULL_CONFIG_B		0x1A
#define ADP5589_RPULL_CONFIG_C		0x1B
#define ADP5589_RPULL_CONFIG_D		0x1C
#define ADP5589_RPULL_CONFIG_E		0x1D
#define ADP5589_GPI_INT_LEVEL_A		0x1E
#define ADP5589_GPI_INT_LEVEL_B		0x1F
#define ADP5589_GPI_INT_LEVEL_C		0x20
#define ADP5589_GPI_EVENT_EN_A		0x21
#define ADP5589_GPI_EVENT_EN_B		0x22
#define ADP5589_GPI_EVENT_EN_C		0x23
#define ADP5589_GPI_INTERRUPT_EN_A	0x24
#define ADP5589_GPI_INTERRUPT_EN_B	0x25
#define ADP5589_GPI_INTERRUPT_EN_C	0x26
#define ADP5589_DEBOUNCE_DIS_A		0x27
#define ADP5589_DEBOUNCE_DIS_B		0x28
#define ADP5589_DEBOUNCE_DIS_C		0x29
#define ADP5589_GPO_DATA_OUT_A		0x2A
#define ADP5589_GPO_DATA_OUT_B		0x2B
#define ADP5589_GPO_DATA_OUT_C		0x2C
#define ADP5589_GPO_OUT_MODE_A		0x2D
#define ADP5589_GPO_OUT_MODE_B		0x2E
#define ADP5589_GPO_OUT_MODE_C		0x2F
#define ADP5589_GPIO_DIRECTION_A	0x30
#define ADP5589_GPIO_DIRECTION_B	0x31
#define ADP5589_GPIO_DIRECTION_C	0x32
#define ADP5589_UNLOCK1				0x33
#define ADP5589_UNLOCK2				0x34
#define ADP5589_EXT_LOCK_EVENT		0x35
#define ADP5589_UNLOCK_TIMERS		0x36
#define ADP5589_LOCK_CFG			0x37
#define ADP5589_RESET1_EVENT_A		0x38
#define ADP5589_RESET1_EVENT_B		0x39
#define ADP5589_RESET1_EVENT_C		0x3A
#define ADP5589_RESET2_EVENT_A		0x3B
#define ADP5589_RESET2_EVENT_B		0x3C
#define ADP5589_RESET_CFG			0x3D
#define ADP5589_PWM_OFFT_LOW		0x3E
#define ADP5589_PWM_OFFT_HIGH		0x3F
#define ADP5589_PWM_ONT_LOW			0x40
#define ADP5589_PWM_ONT_HIGH		0x41
#define ADP5589_PWM_CFG				0x42
#define ADP5589_CLOCK_DIV_CFG		0x43
#define ADP5589_LOGIC_1_CFG			0x44
#define ADP5589_LOGIC_2_CFG			0x45
#define ADP5589_LOGIC_FF_CFG		0x46
#define ADP5589_LOGIC_INT_EVENT_EN	0x47
#define ADP5589_POLL_PTIME_CFG		0x48
#define ADP5589_PIN_CONFIG_A		0x49
#define ADP5589_PIN_CONFIG_B		0x4A
#define ADP5589_PIN_CONFIG_C		0x4B
#define ADP5589_PIN_CONFIG_D		0x4C
#define ADP5589_GENERAL_CFG			0x4D
#define ADP5589_INT_EN				0x4E

/* ID Register */
#define ADP5589_DEVICE_ID_MASK	0xF
#define ADP5589_MAN_ID_MASK		0xF
#define ADP5589_MAN_ID_SHIFT	4
#define ADP5589_MAN_ID			0x02

/* GENERAL_CFG Register */
#define OSC_EN		(1 << 7)
#define CORE_CLK(x)	(((x) & 0x3) << 5)
#define LCK_TRK_LOGIC	(1 << 4)	/* ADP5589 only */
#define LCK_TRK_GPI	(1 << 3)	/* ADP5589 only */
#define INT_CFG		(1 << 1)
#define RST_CFG		(1 << 0)

/* INT_EN Register */
#define LOGIC2_IEN	(1 << 5)	/* ADP5589 only */
#define LOGIC1_IEN	(1 << 4)
#define LOCK_IEN	(1 << 3)	/* ADP5589 only */
#define OVRFLOW_IEN	(1 << 2)
#define GPI_IEN		(1 << 1)
#define EVENT_IEN	(1 << 0)

/* Interrupt Status Register */
#define LOGIC2_INT	(1 << 5)	/* ADP5589 only */
#define LOGIC1_INT	(1 << 4)
#define LOCK_INT	(1 << 3)	/* ADP5589 only */
#define OVRFLOW_INT	(1 << 2)
#define GPI_INT		(1 << 1)
#define EVENT_INT	(1 << 0)

/* STATUS Register */
#define LOGIC2_STAT	(1 << 7)	/* ADP5589 only */
#define LOGIC1_STAT	(1 << 6)
#define LOCK_STAT	(1 << 5)	/* ADP5589 only */
#define KEC			0xF

/* PIN_CONFIG_D Register */
#define C4_EXTEND_CFG	(1 << 6)	/* RESET2 */
#define R4_EXTEND_CFG	(1 << 5)	/* RESET1 */

/* LOCK_CFG */
#define LOCK_EN		(1 << 0)

#define PTIME_MASK	0x3
#define LTIME_MASK	0x3		/* ADP5589 only */

/* Key Event Register xy */
#define KEY_EV_PRESSED		(1 << 7)
#define KEY_EV_MASK			(0x7F)

#define KEYP_MAX_EVENT		16
#define ADP5589_MAXGPIO		19
#define ADP5585_MAXGPIO		11 /* 10 on the ADP5585-01, 11 on ADP5585-02 */

#define ADP5589_NUM_BANKS 2U



/**
 * Get values of adp5589 input port
 *
 * @param[in] i2c: I2C device to use
 * @param[out] result: Pin input values of adp5589 device
 * @return @ref zos_result_t
 */
zos_result_t adp5589_get_input_values(const zos_i2c_device_t* i2c, uint8_t* result);
/**
 * Get values of adp5589 output port
 *
 * @param[in] i2c: I2C device to use
 * @param[out] result: Pin output values of adp5589 device
 * @return @ref zos_result_t
 */
zos_result_t adp5589_get_output_values(const zos_i2c_device_t* i2c, uint8_t* result);

/**
 * Set values of adp5589 port
 *
 * @param[in] i2c: I2C device to use
 * @param[in] values : Pin output values to set
 * @return @ref zos_result_t
 */
zos_result_t adp5589_set_values(const zos_i2c_device_t* i2c, const uint8_t* values);

/**
 * Get directions of adp5589 port
 *
 * @param[in] i2c: I2C device to use
 * @param[out] result: Pin directions (1=Input, 0=Output)
 * @return @ref zos_result_t
 */
zos_result_t adp5589_get_directions(const zos_i2c_device_t* i2c, uint8_t* result);

/**
 * Set directions of adp5589 port
 *
 * @param[in] i2c: I2C device to use
 * @param[in] directions : Pin directions to set (1=Input, 0=Output)
 * @return @ref zos_result_t
 */
zos_result_t adp5589_set_directions(const zos_i2c_device_t* i2c, const uint8_t* directions);

/**
 * Get polarities of adp5589 port
 *
 * @param[in] i2c: I2C device to use
 * @param[out] result: Pin polarities
 * @return @ref zos_result_t
 */
zos_result_t adp5589_get_polarities(const zos_i2c_device_t* i2c, uint8_t* result);

/**
 * Set polarities of adp5589 port
 *
 * @param[in] i2c: I2C device to use
 * @param[in] polarities : Pin polarities to set (0=Active HIGH, 1=Active LOW)
 * @return @ref zos_result_t
 */
zos_result_t adp5589_set_polarities(const zos_i2c_device_t* i2c, const uint8_t* polarities);

#endif /* adp5589_DRIVER_H_ */
