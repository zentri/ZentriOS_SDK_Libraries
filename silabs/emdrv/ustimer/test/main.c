/**************************************************************************//**
 * @file
 * @brief
 * @version 3.20.5
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "unity.h"
#if defined(UNITY_TEST_REPORT_SERIAL)
#include "unity_usart.h"
#else
#include "unity_swo.h"
#endif

#include "ustimer.h"


#if defined (DWT)
  #define USTIMESTAMP_GET    ( DWT->CYCCNT / refClk )
  #define USTIMESTAMP_INIT() { DWT->CTRL |=1; DWT->CYCCNT = 0;  \
    refClk = SystemHFClockGet() / 1000000; }
  #define USTIMESTAMP_INACCURACY  (5)
  static uint32_t refClk;
#else
  extern void     USTIMESTAMP32_Init(void);
  extern uint32_t USTIMESTAMP32_Get(void);
  static uint32_t ustimestamp32_GetStopStart()
  {
    uint32_t retval;
    TIMER1->CMD = TIMER_CMD_STOP;
    retval=USTIMESTAMP32_Get();
    TIMER1->CMD = TIMER_CMD_START;
    return retval;
  }
  #define USTIMESTAMP_INIT()      (USTIMESTAMP32_Init())
  #define USTIMESTAMP_GET         (ustimestamp32_GetStopStart())
  #define USTIMESTAMP_INACCURACY  (50)
#endif

#if 0
/* Set #if 1 above to enable toggling of a GPIO signal in order to measure the
   timers with a logic analyzer. */
#include "em_gpio.h"
#define TOGGLE_GPIO
#endif
#ifdef TOGGLE_GPIO
#define TIMING_SIGNAL_GPIO_PORT         (gpioPortC)
#define TIMING_SIGNAL_GPIO_PIN          (0)
#endif

static void test_USTIMER( void );
static void test_short_delays( void );
static void test_short_delays_intsafe( void );
static void test_long_delays( void );
static void test_long_delays_intsafe( void );

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Enable unity test report output via SWO (Cortex-M3/M4) or
     USART (Cortex-M0+) */
#if defined(UNITY_TEST_REPORT_SERIAL)
  UNITY_UsartSetup();
#else
  SWO_Setup();
#endif

  /* Run test */
  test_USTIMER();

  /* Mark end of test report. */
  UnityPrint( "ENDSWO" );

  return 0;
}

/**************************************************************************//**
 * @brief  Main test function
 *****************************************************************************/
static void test_USTIMER( void )
{
  UnityBeginGroup( "USTIMER" );

#ifdef TOGGLE_GPIO
  CMU_ClockEnable( cmuClock_GPIO, true );
  GPIO_PinModeSet(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN, gpioModePushPull, 0);
  GPIO_PinOutClear(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif

  /* Initialize reference clock */
  USTIMESTAMP_INIT();

  USTIMER_Init();

  RUN_TEST( test_short_delays, __LINE__ );
  RUN_TEST( test_short_delays_intsafe, __LINE__ );

  RUN_TEST( test_long_delays, __LINE__ );
  RUN_TEST( test_long_delays_intsafe, __LINE__ );

  USTIMER_DeInit();

  UnityEnd();
}

static void test_short_delays( void )
{
  uint32_t start, stop, elapsed;

  start = USTIMESTAMP_GET;
#ifdef TOGGLE_GPIO
  GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif
  USTIMER_DelayIntSafe( 50 );
#ifdef TOGGLE_GPIO
  GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif
  stop  = USTIMESTAMP_GET;

  elapsed = stop - start;

  /* Allow up to 100% deviation on short delays. */
  TEST_ASSERT_TRUE( elapsed > 50 );
  TEST_ASSERT_TRUE( elapsed < (100 + USTIMESTAMP_INACCURACY));
}

static void test_short_delays_intsafe( void )
{
  uint32_t start, stop, elapsed;

  start = USTIMESTAMP_GET;
#ifdef TOGGLE_GPIO
  GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif
  USTIMER_Delay( 50 );
#ifdef TOGGLE_GPIO
  GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif
  stop  = USTIMESTAMP_GET;

  elapsed = stop - start;

  /* Allow up to 100% deviation on short delays. */
  TEST_ASSERT_TRUE( elapsed > 50 );
  TEST_ASSERT_TRUE( elapsed < (100 + USTIMESTAMP_INACCURACY) );
}

static void test_long_delays( void )
{
  uint32_t start, stop;
  uint64_t elapsed;

  start = USTIMESTAMP_GET;
  USTIMER_DelayIntSafe( 100000 );
  stop  = USTIMESTAMP_GET;

  elapsed = (uint64_t) stop - start;

  /* Allow 1% deviation. */
  TEST_ASSERT_TRUE( elapsed > 100000 );
  TEST_ASSERT_TRUE( elapsed < (101000 + USTIMESTAMP_INACCURACY) );
}

static void test_long_delays_intsafe( void )
{
  uint32_t start, stop;
  uint64_t elapsed;

  start = USTIMESTAMP_GET;
  USTIMER_Delay( 100000 );
  stop  = USTIMESTAMP_GET;

  elapsed = (uint64_t)stop - start;

  /* Allow 1% deviation. */
  TEST_ASSERT_TRUE( elapsed > 100000 );
  TEST_ASSERT_TRUE( elapsed < (101000 + USTIMESTAMP_INACCURACY) );
}
