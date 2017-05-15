/**************************************************************************//**
 * @file main.c
 * @brief RTC test for EFM32
 * @version 1.6.1
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
#include "em_device.h"

#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "unity.h"
#if defined( GPIO_ROUTE_SWOPEN ) || defined( GPIO_ROUTEPEN_SWVPEN )
#include "unity_swo.h"
#else
#include "unity_usart.h"
#endif

#ifdef __CROSSWORKS_ARM
#include "syscalls_crossworks.h"
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/* Test function prototypes. */
void main_test_RTCDRV(void);

uint32_t rstCause;

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Enable unity test report output via SWO (Cortex-M3/M4) or
     USART (Cortex-M0+) */
#if defined( GPIO_ROUTE_SWOPEN ) || defined( GPIO_ROUTEPEN_SWVPEN )
  SWO_Setup();
#else
  UNITY_UsartSetup();
#endif

  /* Enable clock for GPIO module */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Enable clock for RTC. */
  /* Enable LE core clock to ensure LE modules are accessible. */
  CMU_ClockEnable(cmuClock_CORELE, true);
  /* Enable cfg->source as LFACLK in CMU (will also enable oscillator if
  * not enabled). */
#if defined( RTCC_PRESENT ) && ( RTCC_COUNT == 1 )
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
#endif
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

  /* Run test suits */
  main_test_RTCDRV();
  UnityPrint("ENDSWO");

  return 0;
}
