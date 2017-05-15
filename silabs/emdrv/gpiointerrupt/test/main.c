/**************************************************************************//**
 * @file main.c
 * @brief GPIO interrupt dispatcher test for EFM32
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "gpiointerrupt.h"

#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "unity.h"

#if defined( UNITY_TEST_REPORT_SERIAL )
#include "unity_usart.h"
#include "unity_conf.h"
#else
#include "unity_swo.h"
#endif

/* An array to track if given pin callback was called */
volatile uint8_t pinInt[16];

/**************************************************************************//**
 * @brief  Gpio callback called when pin interrupt was triggered.
 * @param pin - pin number (0-15)
 *****************************************************************************/
void gpioCallback(uint8_t pin)
{
  pinInt[pin]++;
}

/**************************************************************************//**
 * @brief  Test register a callback on given pin and triggers the interrupt on
 *  that pin. Validates that callback was called. Unregister the callback and
 *  triggers the interrupt once again. Validates that callback was not called.
 * @param pin - pin to test (0-15)
 *****************************************************************************/
void test_PinCallbackRegisterUnregister(volatile uint8_t pin)
{
  int i;
  for(i = 0; i < 16; i++) {
    pinInt[i] = 0;
  }
  GPIO_IntEnable(1<<pin);
  GPIOINT_CallbackRegister(pin,gpioCallback);

  GPIO_IntSet(1<<pin);
  __NOP();
  __NOP();
  TEST_ASSERT_EQUAL(1, pinInt[pin]);

  for(i = 0; i < 16; i++) {
    pinInt[i] = 0;
  }
  GPIOINT_CallbackUnRegister(pin);

  for(i = 0; i < 16; i++) {
    pinInt[i] = 0;
  }
  TEST_ASSERT_EQUAL(0, pinInt[pin]);

  GPIO_IntDisable(1<<pin);

}

/**************************************************************************//**
 * @brief  Test validates if all callbacks are triggered in case of multiple
 * interrupts being handled.
 *****************************************************************************/
void test_AllInterrupts(void)
{
  volatile uint8_t i;
  for(i = 0; i < 16; i++) {
    pinInt[i] = 0;
  }
  GPIO_IntEnable(_GPIO_IEN_EXT_MASK);
  for( i = 0; i < 16; i++)
  {
    GPIOINT_CallbackRegister(i,gpioCallback);
  }

  GPIO_IntSet(_GPIO_IFS_EXT_MASK);

  for( i = 0; i < 16; i++)
  {
    TEST_ASSERT_EQUAL(1, pinInt[i]);
    pinInt[i] = 0;
  }

  GPIO_IntDisable(_GPIO_IEN_EXT_MASK);

}
/**************************************************************************//**
 *
 * @brief  Test validates only specified callbacks are triggered if multiple
 * interrupts are being handled.
 *****************************************************************************/
void test_MultipleInterrupts(void)
{
  volatile uint8_t i;
  for(i = 0; i < 16; i++) {
    pinInt[i] = 0;
  }
  GPIO_IntEnable(1<<1 | 1<<3 | 1<<8);

  GPIOINT_CallbackRegister(1,gpioCallback);
  GPIOINT_CallbackRegister(3,gpioCallback);
  GPIOINT_CallbackRegister(8,gpioCallback);

  GPIO_IntSet(1<<1 | 1<<3 | 1<<8);

  for ( i = 0; i < 16; i++)
  {
    if( (i == 1) || (i == 3) || (i == 8))
    {
      TEST_ASSERT((pinInt[i] == 1));
    }
    else
    {
      TEST_ASSERT((pinInt[i] == 0));
    }
    pinInt[i] = 0;
  }

  GPIO_IntDisable(_GPIO_IEN_EXT_MASK);

}
/**************************************************************************//**
 * @brief  Test validates if all pins callbacks can be handled - registering and
 * unregistering is working.
 *****************************************************************************/
void test_CallbackRegisterUnregister(void)
{
  volatile uint8_t i;
  for( i = 0; i < 16; i++)
  {
    test_PinCallbackRegisterUnregister(i);
  }
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
#if defined( UNITY_TEST_REPORT_SERIAL )
  UNITY_UsartSetup();
#else
  SWO_Setup();
#endif

  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Enable clock for GPIO module */
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIOINT_Init();

  /* Setup Unity, and start. */
  UnityBeginGroup("GPIOINTERRUPT");

  /* Run Unity tests. */
  RUN_TEST(test_CallbackRegisterUnregister, __LINE__);
  RUN_TEST(test_MultipleInterrupts, __LINE__);
  RUN_TEST(test_AllInterrupts, __LINE__);

  /* Write summary. */
  UnityEnd();
  UnityPrint("ENDSWO");

  return 0;
}
