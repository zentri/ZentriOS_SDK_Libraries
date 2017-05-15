/**************************************************************************//**
 * @file main.c
 * @brief Energy Modes driver test for EFM32
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

#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_rmu.h"
#include "em_gpio.h"
#include "em_timer.h"

#if defined(LETIMER_PRESENT)
#include "em_letimer.h"
#endif

#if defined(BURTC_PRESENT)
#include "em_burtc.h"
#endif

#if defined(RTCC_PRESENT)
#include "em_rtcc.h"
#endif

#include "sleep.h"

#include "unity.h"
#if defined(UNITY_TEST_REPORT_SERIAL)
#include "unity_usart.h"
#else
#include "unity_swo.h"
#endif

#define ERASED_STATE    (SLEEP_EnergyMode_t) -1
/* Callbacks arguments */
static SLEEP_EnergyMode_t sleepEnter       = ERASED_STATE;
static SLEEP_EnergyMode_t sleepExit        = ERASED_STATE;
static SLEEP_EnergyMode_t sleepEnterOnExit = ERASED_STATE;
static SLEEP_EnergyMode_t sleepExitOnEnter = ERASED_STATE;

#if defined(RMU_RSTCAUSE_EM4WURST)
#define RMU_EM4WAKEUP_FLAG         RMU_RSTCAUSE_EM4WURST
#elif defined(RMU_RSTCAUSE_EM4RST)
#define RMU_EM4WAKEUP_FLAG         RMU_RSTCAUSE_EM4RST
#else
#define RMU_EM4WAKEUP_FLAG         0
#endif

#if defined(BURTC_PRESENT)
/***************************************************************************//**
 * @brief
 *   Initializes BURTC. Used for testing EM4 entry on Platform 1.
 ******************************************************************************/
void BURTC_setup(void)
{
  /* Create burtcInit struct and fill with default values */
  BURTC_Init_TypeDef  burtcInit = BURTC_INIT_DEFAULT;
  EMU_EM4Init_TypeDef em4Init   = EMU_EM4INIT_DEFAULT;

  /* Release reset line to backup domain. This is needed
   * before the CPU can access BURTC registers */
  RMU_ResetControl(rmuResetBU, (RMU_ResetMode_TypeDef)0);

  /* Configuration */
  burtcInit.enable       = false;
  burtcInit.mode         = burtcModeEM4;
  burtcInit.debugRun     = true;
  burtcInit.clkSel       = burtcClkSelULFRCO;
  burtcInit.clkDiv       = burtcClkDiv_2;
  burtcInit.timeStamp    = false;
  burtcInit.compare0Top  = true;
  burtcInit.lowPowerMode = burtcLPDisable;
  burtcInit.lowPowerComp = 0;

  /* Initialize BURTC with burtcInit struct */
  BURTC_Init(&burtcInit);

  /* Set compare value */
  BURTC_CompareSet(0, 0);

  BURTC_IntClear(BURTC_IF_COMP0);
  BURTC_IntEnable(BURTC_IF_COMP0);
  NVIC_ClearPendingIRQ(BURTC_IRQn);
  NVIC_EnableIRQ(BURTC_IRQn);

  EMU_EM4Init(&em4Init);
}

/***************************************************************************//**
 * @brief
 *   BURTC Irq handler.
 * @note
 *   Disables next BURTC irqs and stops BURTC.
 ******************************************************************************/
void BURTC_IRQHandler(void)
{
  BURTC_IntClear(BURTC_IF_COMP0);
  NVIC_ClearPendingIRQ(BURTC_IRQn);
  BURTC_Enable(false);
}
#endif

#if defined(RTCC_PRESENT)
/***************************************************************************//**
 * @brief
 *   Initializes RTCC & EMU. Used for testing EM4H entry on Platform 2.
 ******************************************************************************/
void RTCC_setup(void)
{
  /* Enable RTCC clock */
  CMU_ClockEnable(cmuClock_RTCC, true);

  /* Initialize RTCC */
  RTCC_Init_TypeDef init = RTCC_INIT_DEFAULT;

  init.enable = false;
  init.cntWrapOnCCV1 = false;
  init.presc = rtccCntPresc_1;

  RTCC_Init(&init);

  RTCC->CC[1].CTRL |= RTCC_CC_CTRL_MODE_OUTPUTCOMPARE;

  /* Enable interrupt */
  NVIC_EnableIRQ(RTCC_IRQn);
  RTCC_IntEnable(RTCC_IEN_CC1);

  /* Enable RTCC wakeup from EM4 */
  RTCC_EM4WakeupEnable(true);

  /* Configure EM4 state to EM4H */
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Hibernate;

  EMU_EM4Init(&em4Init);
}

/***************************************************************************//**
 * @brief
 *   RTCC Irq handler.
 * @note
 *   Disables next RTCC irqs and stops RTCC.
 ******************************************************************************/
void RTCC_IRQHandler(void)
{
  RTCC_IntClear(RTCC_IF_CC1);
  NVIC_ClearPendingIRQ(RTCC_IRQn);
  RTCC_Enable(false);
}
#endif

#if defined(LETIMER_PRESENT)
/***************************************************************************//**
 * @brief
 *   LETIMER0 Irq handler.
 * @note
 *   Disables LETIMER0 irqs and stops LETIMER0.
 ******************************************************************************/
void LETIMER0_IRQHandler(void)
{
  LETIMER_Enable(LETIMER0, false);
  LETIMER_IntClear(LETIMER0, LETIMER_IFC_REP0);
  NVIC_ClearPendingIRQ(LETIMER0_IRQn);
}

/***************************************************************************//**
 * @brief
 *   Initializes LETIMER.
 * @note
 *   This function init Low Energy Timer0.
 ******************************************************************************/
void LETIMER_setup(void)
{
  /* Set configurations for the LETIMER unit. */
  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;

  init.enable = false;
  init.comp0Top = true;
  init.ufoa0 = letimerUFOAToggle;
  init.repMode = letimerRepeatOneshot;

  /* Enable clock for LETIMER0 */
  CMU_ClockEnable(cmuClock_LETIMER0, true);

  LETIMER_CompareSet(LETIMER0, 0, 32768 / 1024);

  /* Initialize LETIMER0 */
  LETIMER_Init(LETIMER0, &init);

  /* Enable underflow interrupt */
  LETIMER_IntClear(LETIMER0, LETIMER_IFC_REP0);
  LETIMER_IntEnable(LETIMER0, LETIMER_IEN_REP0);
  NVIC_ClearPendingIRQ(LETIMER0_IRQn);
  NVIC_EnableIRQ(LETIMER0_IRQn);
}
#endif

/**************************************************************************//**
 * @brief TIMER0 configuration, generate OF event every second
 *****************************************************************************/
void TIMER_setup(void)
{
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

  /* Enable clock for TIMER0 */
  CMU_ClockEnable(cmuClock_TIMER0, true);

  /* Basic TIMER configuration */
  timerInit.prescale = timerPrescale1024;
  timerInit.enable   = false;
  timerInit.oneShot  = true;
  timerInit.debugRun = true;
  timerInit.mode     = timerModeDown;
  TIMER_Init(TIMER0, &timerInit);

  /* Enable underflow interrupt */
  TIMER_IntClear(TIMER0, TIMER_IFC_UF);
  TIMER_IntEnable(TIMER0, TIMER_IEN_UF);
  NVIC_ClearPendingIRQ(TIMER0_IRQn);
  NVIC_EnableIRQ(TIMER0_IRQn);
}

/***************************************************************************//**
 * @brief
 *   TIMER0 Irq handler.
 * @note
 *   Disables TIMER0 irqs and stops TIMER0.
 ******************************************************************************/
void TIMER0_IRQHandler(void)
{
  TIMER_Enable(TIMER0, false);
  TIMER_IntClear(TIMER0, TIMER_IFC_UF);
  NVIC_ClearPendingIRQ(TIMER0_IRQn);
}

/*************************************************************************//**
* @brief energyAware Designer MCU initialization
*
* This code is generated by the energyAware Designer appliction to configure
* the EFM32GG230F1024 for application specific operation.
*
* The generated code is a starting point, which might require adjustment for
* correct operation. Call this function at early initialization.
*****************************************************************************/
void eADesigner_Init(void)
{
  /* Initialize HFXO */
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  CMU_HFXOInit(&hfxoInit);

  /* Enable clock for GPIO module */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Enable HFXO as high frequency clock, HFCLK */
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize LFXO */
  CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;
  CMU_LFXOInit(&lfxoInit);

  /* Enable LFXO and wait for it to stabilize */
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  /* Enable LE clock for CPU access to LE peripherals registers */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Select LFXO as clock source for LFACLK (i.e. LETIMER) */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

#if defined(_CMU_LFECLKSEL_MASK)
  /* Select ULFRCO as clock source for LFECLK (i.e. RTCC) */
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_ULFRCO);
#endif

#if defined( BURTC_PRESENT )
  /* BURTC will be counting in every mode */
  BURTC_setup();
#endif

#if defined(RTCC_PRESENT)
  /* RTCC will be counting in every mode */
  RTCC_setup();
#endif

#if defined( LETIMER_PRESENT )
  /* LETIMER will be counting in EM2 and lower */
  LETIMER_setup();
#endif

  /* TIMER will be counting in EM1 */
  TIMER_setup();
}


/**************************************************************************//**
 * @brief  Callback funtion to inform that device is going to sleep
 *****************************************************************************/
void CallbackEnterSleep(SLEEP_EnergyMode_t mode)
{
  sleepEnter       = mode;
  sleepExitOnEnter = sleepExit;
}

/**************************************************************************//**
 * @brief  Callback funtion to inform that device exit sleep
 *****************************************************************************/
void CallbackExitSleep(SLEEP_EnergyMode_t mode)
{
  sleepExit        = mode;
  sleepEnterOnExit = sleepEnter;
}

/**************************************************************************//**
 * @brief  Check that callback were called with correct arguments
 *****************************************************************************/
bool ExpectedStateIsCorrect(SLEEP_EnergyMode_t enter,
                            SLEEP_EnergyMode_t exitOnEnter,
                            SLEEP_EnergyMode_t exit,
                            SLEEP_EnergyMode_t enterOnExit)
{
  return((sleepEnter == enter) && (sleepExitOnEnter == exitOnEnter) &&
         (sleepExit == exit) && (sleepEnterOnExit == enterOnExit));
}

/**************************************************************************//**
 * @brief  Check that callback status is erased.
 *****************************************************************************/
bool StatusIsErased(void)
{
  return(ExpectedStateIsCorrect(ERASED_STATE, ERASED_STATE, ERASED_STATE, ERASED_STATE));
}

/**************************************************************************//**
 * @brief  Erase callbacks status
 *****************************************************************************/
void EraseStatus(void)
{
  sleepEnter       = ERASED_STATE;
  sleepExit        = ERASED_STATE;
  sleepEnterOnExit = ERASED_STATE;
  sleepExitOnEnter = ERASED_STATE;
}

/**************************************************************************//**
 * @brief  Enable timer to ignite interrupt after defined time.
 *         Does not work higher than in EM1.
 *****************************************************************************/
inline static void TIMER_EnableCounting(uint32_t time)
{
  TIMER_CounterSet(TIMER0, (CMU_ClockFreqGet(cmuClock_TIMER0) / (1024 * 1000)) * time);
  TIMER_IntClear(TIMER0, TIMER_IFC_UF);
  NVIC_ClearPendingIRQ(TIMER0_IRQn);
  TIMER_Enable(TIMER0, true);
}

inline static void TIMER_DisableCounting(void)
{
  TIMER_Enable(TIMER0, false);
}

#if defined( LETIMER_PRESENT )
/**************************************************************************//**
 * @brief  Enable timer to ignite interrupt after defined time.
 *         Doesn't work higher than in EM2.
 *****************************************************************************/
static void LETIMER_EnableCounting(uint32_t time)
{
  while (LETIMER0->SYNCBUSY)
    ;
  LETIMER_RepeatSet(LETIMER0, 0, time);
  LETIMER_IntClear(LETIMER0, LETIMER_IFC_REP0);
  NVIC_ClearPendingIRQ(LETIMER0_IRQn);
  LETIMER_Enable(LETIMER0, true);
}
#endif

#if defined(BURTC_PRESENT)
/**************************************************************************//**
 * @brief  Enable timer to ignite interrupt after defined time.
 *         Works in all energy modes.
 *****************************************************************************/
static void BURTC_EnableCounting(uint32_t time)
{
  BURTC_CompareSet(0, time);
  BURTC_CounterReset();
  BURTC_IntClear(BURTC_IF_COMP0);
  NVIC_ClearPendingIRQ(BURTC_IRQn);
  BURTC_Enable(true);
}
#endif

#if defined(RTCC_PRESENT)
/**************************************************************************//**
 * @brief  Enable timer to ignite interrupt after defined time.
 *         Works in all energy modes.
 * @param[in] time Delay time in milliseconds
 *****************************************************************************/
static void RTCC_EnableCounting(uint32_t time)
{
  /* Set CC1 to supplied value. Since ULFRCO is 1 kHz, ticks <-> ms is 1:1 */
  RTCC_ChannelCCVSet(1, time);
  /* Reset counter */
  RTCC_CounterSet(0);
  RTCC_IntClear(RTCC_IF_CC1);
  NVIC_ClearPendingIRQ(RTCC_IRQn);
  /* Start counting*/
  RTCC_Enable(true);
}
#endif
/**************************************************************************//**
 * @brief  Simple delay using TIMER0 counter.
 *****************************************************************************/
static void Delay(uint32_t time)
{
  SLEEP_SleepBlockBegin(sleepEM2);
  TIMER_EnableCounting(time);
  SLEEP_Sleep();
  SLEEP_SleepBlockEnd(sleepEM2);
}

/**************************************************************************//**
 * @brief  Verify that driver initialization passes correctly
 *****************************************************************************/
void test_Initialization(void)
{
  EraseStatus();
  TEST_ASSERT(StatusIsErased());
  SLEEP_Init(CallbackEnterSleep, CallbackExitSleep);
  /* we don't expect that any callback was executed during initialization */
  TEST_ASSERT(StatusIsErased());
}

/**************************************************************************//**
 * @brief  Verify that driver blocks EM2..4 correctly
 *****************************************************************************/
void test_BlockingEM2(void)
{
  SLEEP_EnergyMode_t status;

  EraseStatus();
  TEST_ASSERT(StatusIsErased());
  SLEEP_SleepBlockBegin(sleepEM2);
  TIMER_EnableCounting(100);
  status = SLEEP_Sleep();
  TIMER_DisableCounting();
  SLEEP_SleepBlockEnd(sleepEM2);
  /* we expect that callback was executed with information about entering EM1 */
  TEST_ASSERT(ExpectedStateIsCorrect(sleepEM1, ERASED_STATE, sleepEM1, sleepEM1));
  TEST_ASSERT(sleepEM1 == status);
}

/**************************************************************************//**
 * @brief  Verify that driver blocks EM2..4 correctly
 *****************************************************************************/
void test_BlockingEM2ManyTimes(void)
{
  SLEEP_EnergyMode_t status;

  EraseStatus();
  TEST_ASSERT(StatusIsErased());
  SLEEP_SleepBlockBegin(sleepEM2);
  SLEEP_SleepBlockBegin(sleepEM2);
  SLEEP_SleepBlockBegin(sleepEM2);

  TIMER_EnableCounting(100);
  status = SLEEP_Sleep();
  TIMER_DisableCounting();
  /* we expect that callback was executed with information about entering EM1 */
  TEST_ASSERT(true == ExpectedStateIsCorrect(sleepEM1, ERASED_STATE, sleepEM1, sleepEM1));
  TEST_ASSERT(sleepEM1 == status);

  EraseStatus();
  TEST_ASSERT(true == StatusIsErased());
  SLEEP_SleepBlockEnd(sleepEM2);
  TIMER_EnableCounting(100);
  status = SLEEP_Sleep();
  TIMER_DisableCounting();
  /* we expect that callback was executed with information about entering EM1 */
  TEST_ASSERT(true == ExpectedStateIsCorrect(sleepEM1, ERASED_STATE, sleepEM1, sleepEM1));
  TEST_ASSERT(sleepEM1 == status);

  EraseStatus();
  SLEEP_SleepBlockEnd(sleepEM2);
  TIMER_EnableCounting(100);
  status = SLEEP_Sleep();
  TIMER_DisableCounting();
  /* we expect that callback was executed with information about entering EM1 */
  TEST_ASSERT(true == ExpectedStateIsCorrect(sleepEM1, ERASED_STATE, sleepEM1, sleepEM1));
  TEST_ASSERT(sleepEM1 == status);

  SLEEP_SleepBlockEnd(sleepEM2);
}

#if defined(LETIMER_PRESENT)
/**************************************************************************//**
 * @brief  Verify that driver blocks EM3..4 correctly
 *****************************************************************************/
void test_BlockingEM3(void)
{
  SLEEP_EnergyMode_t status;

  EraseStatus();
  TEST_ASSERT(StatusIsErased());
  SLEEP_SleepBlockBegin(sleepEM2);
  SLEEP_SleepBlockBegin(sleepEM3);
  SLEEP_SleepBlockEnd(sleepEM2); /* verify correctness of handling EM2Block */
  LETIMER_EnableCounting(100);
  status = SLEEP_Sleep();
  SLEEP_SleepBlockEnd(sleepEM3);

  /* we expect that callback was executed with information about entering EM1 */
  TEST_ASSERT(ExpectedStateIsCorrect(sleepEM2, ERASED_STATE, sleepEM2, sleepEM2));
  TEST_ASSERT(sleepEM2 == status);
}
#endif

#if defined(LETIMER_PRESENT)
/**************************************************************************//**
 * @brief  Verify that driver blocks EM3..4 correctly
 *****************************************************************************/
void test_BlockingEM3ManyTimes(void)
{
  SLEEP_EnergyMode_t status;

  EraseStatus();
  TEST_ASSERT(StatusIsErased());

  SLEEP_SleepBlockBegin(sleepEM3);
  SLEEP_SleepBlockBegin(sleepEM3);
  SLEEP_SleepBlockBegin(sleepEM3);

  LETIMER_EnableCounting(100);
  status = SLEEP_Sleep();

  /* we expect that callback was executed with information about entering EM2 */
  TEST_ASSERT(true == ExpectedStateIsCorrect(sleepEM2, ERASED_STATE, sleepEM2, sleepEM2));
  TEST_ASSERT(sleepEM2 == status);

  EraseStatus();
  TEST_ASSERT(true == StatusIsErased());
  SLEEP_SleepBlockEnd(sleepEM3);
  LETIMER_EnableCounting(100);
  status = SLEEP_Sleep();

  /* we expect that callback was executed with information about entering EM2 */
  TEST_ASSERT(true == ExpectedStateIsCorrect(sleepEM2, ERASED_STATE, sleepEM2, sleepEM2));
  TEST_ASSERT(sleepEM2 == status);

  EraseStatus();
  SLEEP_SleepBlockEnd(sleepEM3);
  LETIMER_EnableCounting(100);
  status = SLEEP_Sleep();

  /* we expect that callback was executed with information about entering EM2 */
  TEST_ASSERT(true == ExpectedStateIsCorrect(sleepEM2, ERASED_STATE, sleepEM2, sleepEM2));
  TEST_ASSERT(sleepEM2 == status);

  SLEEP_SleepBlockEnd(sleepEM3);
}
#endif

#if defined(BURTC_PRESENT) || defined(RTCC_PRESENT)
/**************************************************************************//**
 * @brief  Verify that driver enters into EM3 correctly
 *****************************************************************************/
void test_EnteringEM3(void)
{
  SLEEP_EnergyMode_t status;

  EraseStatus();
  TEST_ASSERT(StatusIsErased());

#if defined(BURTC_PRESENT)
  BURTC_EnableCounting(100);
#else
  RTCC_EnableCounting(100);
#endif
  status = SLEEP_Sleep();

  /* we expect that callback was executed with information about entering EM3 */
  TEST_ASSERT(ExpectedStateIsCorrect(sleepEM3, ERASED_STATE, sleepEM3, sleepEM3));
  TEST_ASSERT(sleepEM3 == status);
}
#endif

#if defined(BURTC_PRESENT) || defined(RTCC_PRESENT)
/**************************************************************************//**
 * @brief  Verify that driver enters EM4 correctly
 *****************************************************************************/
void test_EnteringEM4(void)
{
  EraseStatus();
  TEST_ASSERT(StatusIsErased());

#if defined(BURTC_PRESENT)
  BURTC_EnableCounting(100);
#else
  RTCC_EnableCounting(100);
#endif

  /* keep tests statistics in retention registers */
#if defined(BURTC_PRESENT)
  BURTC_RetRegSet(1, UnityGet()->NumberOfTests);
  BURTC_RetRegSet(2, UnityGet()->TestFailures);
  BURTC_RetRegSet(3, UnityGet()->TestIgnores);
#else
  RTCC->RET[0].REG = UnityGet()->NumberOfTests;
  RTCC->RET[1].REG = UnityGet()->TestFailures;
  RTCC->RET[2].REG = UnityGet()->TestIgnores;
#endif
  /* force to enter EM4, correct behavior is to reset MCU after 100ms */
  SLEEP_ForceSleepInEM4();

  /* if we are here it means we were not in EM4 */
  TEST_ASSERT(false);
}

/**************************************************************************//**
 * @brief  Used to show that system was woken up from EM4 correctly
 *****************************************************************************/
void test_ExitFromEM4(void)
{ /* this routine is just to inform that successfully woken-up from EM4 */
  /* It should have empty body, just fact we entered it means we were in EM4 */
}

/**************************************************************************//**
 * @brief  Verify that driver noticed fact that was in EM4 earlier
 *****************************************************************************/
void test_InitializationAfterEM4(void)
{
  EraseStatus();
  TEST_ASSERT(StatusIsErased());
  SLEEP_Init(CallbackEnterSleep, CallbackExitSleep);
  /* we expect that callback informing about exit from EM4 was executed */
  TEST_ASSERT_EQUAL(ERASED_STATE, sleepEnter);
  TEST_ASSERT_EQUAL(ERASED_STATE, sleepEnterOnExit);
  TEST_ASSERT_EQUAL(sleepEM4, sleepExit);
  TEST_ASSERT_EQUAL(ERASED_STATE, sleepExitOnEnter);
}

static void SleepTestEM4Wakeup(void)
{
  /* We just experienced reset from BURTC/RTCC to exit from EM4,
   * so let's continue tests */

  /* restore Unity internal variables to correctly display tests results summary */
  UnityGet()->TestFile = "SLEEP";
#if defined(BURTC_PRESENT)
  BURTC_Enable(true);
  UnityGet()->NumberOfTests = BURTC_RetRegGet(1) - 1;
  UnityGet()->TestFailures  = BURTC_RetRegGet(2);
  UnityGet()->TestIgnores   = BURTC_RetRegGet(3);
  BURTC_Enable(false);
#elif defined(RTCC_PRESENT)
  RTCC_Enable(true);
  UnityGet()->NumberOfTests = RTCC->RET[0].REG - 1;
  UnityGet()->TestFailures  = RTCC->RET[1].REG;
  UnityGet()->TestIgnores   = RTCC->RET[2].REG;
  RTCC_Enable(false);
#endif
  RUN_TEST(test_ExitFromEM4, __LINE__);
  RUN_TEST(test_InitializationAfterEM4, __LINE__);
}
#endif


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  bool em4wakeup;

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
  /* Workaround for Dumbo latchup errata PGDUMBO-3310 */
  EMU_UnlatchPinRetention();
#endif

  /* Chip revision alignment and errata fixes */
  CHIP_Init();

#if defined(_EMU_DCDCCTRL_MASK)
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
#endif

  eADesigner_Init();

  /* Enable unity test report output via SWO (Cortex-M3/M4) or
     USART (Cortex-M0+) */
#if defined(UNITY_TEST_REPORT_SERIAL)
  UNITY_UsartSetup();
#else
  SWO_Setup();
#endif

  /* check if we were in EM4 - if yes it means that this is second loop */
  em4wakeup = RMU_ResetCauseGet() & RMU_EM4WAKEUP_FLAG;

  if (em4wakeup)
  {
#if defined(BURTC_PRESENT) || defined(RTCC_PRESENT)
    SleepTestEM4Wakeup();
#endif
  }
  else
  {
    RMU_ResetCauseClear();

    /* Setup Unity, and start. */
    UnityBeginGroup("SLEEP");

    /* Run Unity tests. */
    RUN_TEST(test_Initialization, __LINE__);
    RUN_TEST(test_BlockingEM2, __LINE__);
    RUN_TEST(test_BlockingEM2ManyTimes, __LINE__);
    Delay(10);
#if defined( LETIMER_PRESENT )
    RUN_TEST(test_BlockingEM3, __LINE__);
    Delay(10);
    RUN_TEST(test_BlockingEM3ManyTimes, __LINE__);
    Delay(10);
#endif
#if defined(BURTC_PRESENT) || defined(RTCC_PRESENT)
    RUN_TEST(test_EnteringEM3, __LINE__);
    /* Note that the following test used to break the SWO output on
     * Series 1 devices. The introduction of mass erase and pin reset
     * before each test run fixed this. */
    Delay(10);
    RUN_TEST(test_EnteringEM4, __LINE__);
#endif
  }

  /* Write summary. */
  UnityEnd();
  UnityPrint("ENDSWO");

  return 0;
}
