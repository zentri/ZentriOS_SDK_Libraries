#include <time.h>

#include "em_timer.h"
#include "unity.h"
#include "em_core.h"
#include "em_cmu.h"
#include "em_device.h"
#if defined (LETIMER_PRESENT) && defined(LETIMER_CTRL_RTCC0TEN)
#include "em_letimer.h"
#endif
#include "rtcdriver.h"
#include "sleep.h"

#if 0
/* Set #if 1 above to enable toggling of a GPIO signal in order to measure the
   timers with a logic analyzer. */
#include "em_gpio.h"
#define TOGGLE_GPIO
#endif
#ifdef TOGGLE_GPIO
#define TIMING_SIGNAL_GPIO_PORT         (gpioPortD)
#define TIMING_SIGNAL_GPIO_PIN          (0)
#endif

#define RTC_CLOCK_FREQ    (32768 / 8)
#define RTC_TICK_IN_USEC  (1000000 / RTC_CLOCK_FREQ)

static volatile bool calledback[4];
volatile bool end = false;
uint32_t setTime = 1377170940;
bool execute = false; /* Used in test_stop_first_running */

uint32_t hfClk;
typedef struct
{
  uint32_t timeout;
  uint32_t setTimeout;
  uint32_t timestamp;
} RTCDRVSTM_TimerUT_t;

volatile RTCDRVSTM_TimerUT_t timerUt[3];
uint32_t letimer_irqTimeout;
uint32_t letimer_id;
RTCDRV_Callback_t letimer_clbk = 0;
typedef void (*letimerClbk_t)(void);
letimerClbk_t letimer_Userclbk = 0;

RTCDRV_TimerID_t id0, id1, id2;

#define TIMESTAMP_TBL_SIZE (100)
uint32_t  startTimestamp[TIMESTAMP_TBL_SIZE];
uint32_t  endTimestamp[TIMESTAMP_TBL_SIZE];


#if defined (DWT)
#define GET_MS_TIMESTAMP  ( (DWT->CYCCNT) / hfClk )
#define GET_US_TIMESTAMP  ( (DWT->CYCCNT) / (hfClk / 1000) )
#define INIT_TIMESTAMP()  { DWT->CTRL |=1; DWT->CYCCNT = 0; \
                            hfClk = SystemHFClockGet()/1000; }
#else
extern void     USTIMESTAMP32_Init(void);
extern uint32_t USTIMESTAMP32_Get(void);
#define INIT_TIMESTAMP()  (USTIMESTAMP32_Init())
#define GET_US_TIMESTAMP  (USTIMESTAMP32_Get())
#define GET_MS_TIMESTAMP  (USTIMESTAMP32_Get()/1000)
#endif


/* Set of helper functions which wraps things which are called over and over again. */
void setupTimer( int id, uint32_t timeout,RTCDRV_Callback_t pCbFun)
{
  timerUt[id].timeout = timeout;
  timerUt[id].timestamp = GET_MS_TIMESTAMP;
  RTCDRV_StartTimer(id, rtcdrvTimerTypeOneshot, timerUt[id].timeout, pCbFun, NULL);
}

void setupSecTimer( int id, uint32_t timeout,RTCDRV_Callback_t pCbFun)
{
  timerUt[id].timeout = timeout*1000;
  timerUt[id].timestamp = GET_MS_TIMESTAMP;
  RTCDRV_StartTimer(id, rtcdrvTimerTypeOneshot, timeout*1000, pCbFun, NULL);
}

void setupPeriodicTimer( int id, uint32_t timeout,RTCDRV_Callback_t pCbFun)
{
  timerUt[id].timeout = timeout;
  timerUt[id].timestamp = GET_MS_TIMESTAMP;
  RTCDRV_StartTimer(id, rtcdrvTimerTypePeriodic, timerUt[id].timeout, pCbFun, NULL);
}

void validateTimingUnprecise(int id, uint32_t delta)
{
  char str[2];

  timerUt[id].setTimeout = GET_MS_TIMESTAMP -  timerUt[id].timestamp;

  str[0] = 48+id;
  str[1] = 0;
  UnityPrint("callbackTiming");
  UnityPrint(str);
  UNITY_PRINT_EOL;

  TEST_ASSERT_UINT_WITHIN(delta, timerUt[id].timeout, timerUt[id].setTimeout);
}

void validateTiming(int id)
{
  validateTimingUnprecise(id,40);
}

void callbackTimingBasic(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);
}

void callbackTimingLast(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);

#if defined( EMDRV_RTCDRV_SLEEPDRV_INTEGRATION )
  #if defined( _EFR_DEVICE )
    // Lowest sleep mode shall be EM3
    TEST_ASSERT_EQUAL( sleepEM3, SLEEP_LowestEnergyModeGet() );
  #else
    #if defined( EMDRV_RTCDRV_WALLCLOCK_CONFIG )
      // Lowest sleep mode shall be EM2
      TEST_ASSERT_EQUAL( sleepEM2, SLEEP_LowestEnergyModeGet() );
    #else
      // Lowest sleep mode shall be EM3
      TEST_ASSERT_EQUAL( sleepEM3, SLEEP_LowestEnergyModeGet() );
    #endif
  #endif
#endif

  end = true;
}


/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Allocate 4 Timers
 * 2. Check first 3 Id of Timers.
 * 3. Set Timer0 for 50 ms, Timer1 for 8ms,
 *    Timer2 for 20ms, Timer3 for 90ms
 * 4. Test RTCDRV_TimerActivityGet for all Timers
 * 5. In Timeout callback from Timer1 (which is supposed to fire first)
 *    check if Timer0 expired erlier, check activity of Timer0 and Timer2,
 *    free Timer2 and check activity of it
 * 6. In Timeout callback from Timer0 (which should fire second)
 *    check if Timer1 fired erlier, check Activity of Timer 1
 * 7. In Timeout callback from Timer3 set end of this test case
 */
void callback0(RTCDRV_TimerID_t id, void *user)
{
  bool isRunning;
  (void)user;

  UnityPrint("timer 0 triggered");
  UNITY_PRINT_EOL;
  TEST_ASSERT_EQUAL_INT(0, id);
  calledback[0] = true;
  // Timer 0 is supposed to trigger after timer 1
  TEST_ASSERT_TRUE(calledback[1]);
  RTCDRV_IsRunning(1, &isRunning);
  TEST_ASSERT_FALSE( isRunning );
}

void callback1(RTCDRV_TimerID_t id, void *user)
{
  bool isRunning;
  (void)user;

  UnityPrint("timer 1 triggered");
  UNITY_PRINT_EOL;
  TEST_ASSERT_EQUAL_INT(1, id);
  calledback[1] = true;
  // Timer 1 is supposed to be the first one to trigger
  TEST_ASSERT_FALSE(calledback[0]);

  RTCDRV_IsRunning(0, &isRunning);
  TEST_ASSERT_TRUE( isRunning );
  RTCDRV_IsRunning(2, &isRunning);
  TEST_ASSERT_TRUE( isRunning );

  RTCDRV_FreeTimer(2);

  TEST_ASSERT_EQUAL( ECODE_EMDRV_RTCDRV_TIMER_NOT_ALLOCATED,
                     RTCDRV_IsRunning(2, &isRunning) );
}

void callback2(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  UnityPrint("timer 2 triggered");
  UNITY_PRINT_EOL;
  TEST_ASSERT_EQUAL_INT(2, id);
  calledback[2] = true;
  TEST_FAIL_MESSAGE("Timer 2 was not supposed to trigger, because it is disabled by timer 1");
}

void callbackEnd(RTCDRV_TimerID_t id, void *user)
{
  (void)id;
  (void)user;
  end = true;
}

void test_RTCDRV(void)
{
  bool isRunning;

  end = false;
  RTCDRV_TimerID_t id0, id1, id2, idEnd;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  /* Allocate new instance */
  RTCDRV_AllocateTimer( &id0   );
  RTCDRV_AllocateTimer( &id1   );
  RTCDRV_AllocateTimer( &id2   );
  RTCDRV_AllocateTimer( &idEnd );

  TEST_ASSERT_EQUAL_INT(0, id0);
  TEST_ASSERT_EQUAL_INT(1, id1);
  TEST_ASSERT_EQUAL_INT(2, id2);

  calledback[0]=false;
  calledback[1]=false;
  calledback[2]=false;

  RTCDRV_StartTimer(id0,   rtcdrvTimerTypeOneshot, 50, callback0,   NULL);
  RTCDRV_StartTimer(id1,   rtcdrvTimerTypeOneshot,  8, callback1,   NULL);
  RTCDRV_StartTimer(id2,   rtcdrvTimerTypeOneshot, 20, callback2,   NULL);
  RTCDRV_StartTimer(idEnd, rtcdrvTimerTypeOneshot, 90, callbackEnd, NULL);

  RTCDRV_IsRunning(id0, &isRunning);
  TEST_ASSERT_TRUE( isRunning );
  RTCDRV_IsRunning(id1, &isRunning);
  TEST_ASSERT_TRUE( isRunning );
  RTCDRV_IsRunning(id2, &isRunning);
  TEST_ASSERT_TRUE( isRunning );
  RTCDRV_IsRunning(idEnd, &isRunning);
  TEST_ASSERT_TRUE( isRunning );

  while(1)
  {
    if( true == end)
    {
      break;
    }
  }

  TEST_ASSERT_TRUE(calledback[0]);
  TEST_ASSERT_TRUE(calledback[1]);
  TEST_ASSERT_FALSE(calledback[2]);
  RTCDRV_IsRunning(id0, &isRunning);
  TEST_ASSERT_FALSE( isRunning );
  RTCDRV_IsRunning(id1, &isRunning);
  TEST_ASSERT_FALSE( isRunning );
  RTCDRV_IsRunning(id2, &isRunning);
  TEST_ASSERT_FALSE( isRunning );

  RTCDRV_DeInit();
}

void test_time_remaining(void)
{
  RTCDRV_TimerID_t id;
  uint32_t remaining;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  /* Allocate new instance */
  RTCDRV_AllocateTimer(&id);
  RTCDRV_StartTimer(id, rtcdrvTimerTypeOneshot, UINT32_MAX, NULL, NULL);
  RTCDRV_Delay(10);
  RTCDRV_TimeRemaining(id, &remaining);
  RTCDRV_StopTimer(id);

  /* Time remaining should now be ~(0xFFFFFFFF - 10).
   * We allow a 5 ms delta for test to pass. */
  TEST_ASSERT_UINT_WITHIN(5, (UINT32_MAX - 10), remaining);

  RTCDRV_DeInit();
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Test that multiple concurrent timers end in correct order.
 * 2. Multiple timers are kicked off with different end times, given by
 *    CONCURRENT_INTERVAL. The first timer started is the last to end, and the
 *    last timer started is the first to end.
 */


#define NUM_CONCURRENT_TIMERS   (EMDRV_RTCDRV_NUM_TIMERS)
#define CONCURRENT_INTERVAL     (1)

void callbackConcurrentTest(RTCDRV_TimerID_t id, void *user)
{
  bool     isRunning;
  uint32_t i;
  char     str[16];

  (void)user;

  endTimestamp[id] = GET_US_TIMESTAMP;

#ifdef TOGGLE_GPIO
  GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif

  calledback[id] = true;

  // Verify that all timers below 'id' are still running.
  for (i=0; i<id; i++)
  {
    RTCDRV_IsRunning(i, &isRunning);
    TEST_ASSERT_TRUE( isRunning );
    TEST_ASSERT_FALSE(calledback[i]);
  }

  // Verify that all timers above 'id' are not running.
  for (i=id; i<NUM_CONCURRENT_TIMERS; i++)
  {
    RTCDRV_IsRunning(i, &isRunning);
    TEST_ASSERT_FALSE( isRunning );
    if (calledback[i]==false)
    {
      sprintf(str, "Timer %d callback error.", (int)i);
      TEST_ASSERT_TRUE_MESSAGE(calledback[i], str);
    }
  }

  // Timer 0 should be the last to finish. Set the end flag.
  if (0 == id) {
    end = true;
  }
}

void test_concurrent_timers(void)
{
  bool     isRunning;
  int      i;
  uint32_t usecs;
  uint32_t msecs;
  RTCDRV_TimerID_t id[NUM_CONCURRENT_TIMERS];

  UnityPrintf("CONCURRENT TIMERS TEST\n");

#ifdef TOGGLE_GPIO
  GPIO_PinModeSet(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN, gpioModePushPull, 0);
  GPIO_PinOutClear(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  /* Allocate new timer instances */
  for (i=0; i<NUM_CONCURRENT_TIMERS; i++)
  {
    RTCDRV_AllocateTimer( &id[i] );

    TEST_ASSERT_EQUAL_INT(i, id[i]);

    calledback[i]=false;
  }

  /* Start all timers concurrently with different timeouts.
     First timer started, ends last. Last timer started ends first. */
  end = false;
  for (i=0; i<NUM_CONCURRENT_TIMERS; i++)
  {
#ifdef TOGGLE_GPIO
    GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif
    startTimestamp[i] = GET_US_TIMESTAMP;
    RTCDRV_StartTimer(id[i],
                      rtcdrvTimerTypeOneshot,
                      (NUM_CONCURRENT_TIMERS - i) * CONCURRENT_INTERVAL,
                      callbackConcurrentTest,
                      NULL);
  }

  /* Check that all timers are running concurrently. */
  for (i=0; i<NUM_CONCURRENT_TIMERS; i++)
  {
    RTCDRV_IsRunning(id[i], &isRunning);
    TEST_ASSERT_TRUE( isRunning );
  }

  /* Wait for the last timer to complete. */
  while(1)
  {
    if( true == end)
    {
      break;
    }
  }

  /* Check that all timers have called the callback. */
  for (i=0; i<NUM_CONCURRENT_TIMERS; i++)
  {
    TEST_ASSERT_TRUE(calledback[i]);
  }

  /* Check that all timers have completed. */
  for (i=0; i<NUM_CONCURRENT_TIMERS; i++)
  {
    RTCDRV_IsRunning(id[i], &isRunning);
    TEST_ASSERT_FALSE( isRunning );

    usecs = endTimestamp[i] - startTimestamp[i];
    msecs = (NUM_CONCURRENT_TIMERS - i) * CONCURRENT_INTERVAL;

    UnityPrintf("Timer %d of %d ms elapsed %d us.\n", i+1, msecs, usecs);

    uint32_t usecMin = ((msecs*1000) - RTC_TICK_IN_USEC);

    if (usecs < usecMin)
    {
      UnityPrintf("Timer %d ended earlier (%d us) than expected (%d us) min limit (%d us).\n",
                  i+1, usecs, msecs*1000, usecMin);
      TEST_ASSERT(usecs >= usecMin);
    }
    if (usecs >= (msecs+1)*1000)
    {
      UnityPrintf("Timer %d ended later (%d us) than expected (%d us).\n",
                  i+1, usecs, (msecs+1)*1000);
      TEST_ASSERT_UINT_WITHIN(1000, msecs*1000, usecs);
    }
  }

  RTCDRV_DeInit();
}


/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Test a oneshot timer with 1 ms delay which restarts itself in the callback.
 */

static int restartCallbackCounter = 0;

void callbackRestart(RTCDRV_TimerID_t id, void *user)
{
  (void) user;

  endTimestamp[restartCallbackCounter] = GET_US_TIMESTAMP;
#ifdef TOGGLE_GPIO
  GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif
  restartCallbackCounter++;

  if (restartCallbackCounter < TIMESTAMP_TBL_SIZE)
  {
    startTimestamp[restartCallbackCounter] = GET_US_TIMESTAMP;
    RTCDRV_StartTimer(id,
                      rtcdrvTimerTypeOneshot,
                      1,
                      callbackRestart,
                      (void*)restartCallbackCounter);
  }
  else
  {
    end = true;
  }
}

void test_timer_1ms_callback_restart(void)
{
  bool isRunning;
  int  i;
  uint32_t usecs;
  RTCDRV_TimerID_t id;

  UnityPrintf("1MS CALLBACK RESTART TIMER TEST\n");

  INIT_TIMESTAMP();

#ifdef TOGGLE_GPIO
  GPIO_PinModeSet(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN, gpioModePushPull, 0);
  GPIO_PinOutClear(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  /* Allocate new timer instances */
  RTCDRV_AllocateTimer( &id );

  TEST_ASSERT_EQUAL_INT(0, id);

  end = false;
#ifdef TOGGLE_GPIO
  GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif
  startTimestamp[0] = GET_US_TIMESTAMP;
  RTCDRV_StartTimer(id,
                    rtcdrvTimerTypeOneshot,
                    1,
                    callbackRestart,
                    (void*)0);

  RTCDRV_IsRunning(id, &isRunning);
  TEST_ASSERT_TRUE( isRunning );

  while(1)
  {
    if( true == end)
    {
      break;
    }
  }

  RTCDRV_IsRunning(id, &isRunning);
  TEST_ASSERT_FALSE( isRunning );

  // Verify callback timestamps
  for (i=0; i<TIMESTAMP_TBL_SIZE; i++)
  {
    usecs = endTimestamp[i] - startTimestamp[i];
    UnityPrintf("Callback %d called after %d us.\n", i+1, usecs);

    if (usecs < 1000)
    {
      UnityPrintf("Callback %d called earlier (%d us) than expected (%d us).\n",
                  i+1, usecs, 1000);
      TEST_ASSERT(usecs >= 1000);
    }
    if (usecs >= 2000)
    {
      UnityPrintf("Callback %d called later (%d us) than expected (%d us).\n",
                  i+1, usecs, 2000);
      TEST_ASSERT_UINT_WITHIN(1000, 2000, usecs);
    }
  }

  RTCDRV_DeInit();
}


/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Test a periodic timer with 1 ms delay.
 */

static int periodicCallbackCounter = 0;

void callbackPeriodic(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  (void)id;

  if (periodicCallbackCounter < TIMESTAMP_TBL_SIZE)
  {
    endTimestamp[periodicCallbackCounter] = GET_US_TIMESTAMP;
    if (periodicCallbackCounter < TIMESTAMP_TBL_SIZE-1)
      startTimestamp[periodicCallbackCounter+1] =
        endTimestamp[periodicCallbackCounter];
  }
  else
    end = true;

#ifdef TOGGLE_GPIO
  GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif

  periodicCallbackCounter++;
}

void test_timer_1ms_periodic(void)
{
  bool isRunning;
  int  i;
  uint32_t usecs;
  uint32_t msecs;
  uint32_t delta;
  RTCDRV_TimerID_t id;

  UnityPrintf("1MS PERIODIC TIMER TEST\n");

  INIT_TIMESTAMP();

#ifdef TOGGLE_GPIO
  GPIO_PinModeSet(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN, gpioModePushPull, 0);
  GPIO_PinOutClear(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  /* Allocate new timer instances */
  RTCDRV_AllocateTimer( &id );

  TEST_ASSERT_EQUAL_INT(0, id);

  // Clear callback timestamps
  for (i=0; i<TIMESTAMP_TBL_SIZE; i++)
  {
    endTimestamp[i] = startTimestamp[i] = 0;
  }
  periodicCallbackCounter = 0;
  end = false;
#ifdef TOGGLE_GPIO
  GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif
  startTimestamp[0] = GET_US_TIMESTAMP;
  RTCDRV_StartTimer(id,
                    rtcdrvTimerTypePeriodic,
                    1,
                    callbackPeriodic,
                    (void*)0);

  RTCDRV_IsRunning(id, &isRunning);
  TEST_ASSERT_TRUE( isRunning );

  while (!end)
    ;

  RTCDRV_IsRunning(id, &isRunning);
  TEST_ASSERT_TRUE( isRunning );

  // Stop the timer and verify that it has stopped
  RTCDRV_StopTimer(id);
  RTCDRV_IsRunning(id, &isRunning);
  TEST_ASSERT_FALSE( isRunning );

  // Verify callback timestamps
  for (i=0; i<TIMESTAMP_TBL_SIZE; i++)
  {
    msecs = i + 1;
    usecs = endTimestamp[i] - startTimestamp[0];
    delta = (msecs / 40) + 1;  // Allow for 1ms deviation per 40ms.

    UnityPrintf("Callback %d called after %d us.\n", i+1, usecs);

    if (usecs < (msecs-1)*1000)
    {
      UnityPrintf("Callback %d called earlier (%d us) than expected (%d us).\n",
                  i+1, usecs, msecs*1000);
      TEST_ASSERT(usecs >= (msecs-1)*1000);
    }
    if (usecs >= (msecs+delta)*1000)
    {
      UnityPrintf("Callback %d called later (%d us) than expected (%d us).\n",
                  i+1, usecs, (msecs+delta)*1000);
      TEST_ASSERT_UINT_WITHIN(delta*1000, msecs*1000, usecs);
    }
  }

  RTCDRV_DeInit();
}


/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Test a periodic timer with 6 ms delay,, in order to test negative
 *    drift compensations for periodic timers. (6ms gives negative compensation
 *    for Tiny and Giant when the RTC prescaler is 8.)
 */

void test_timer_6ms_periodic(void)
{
  bool isRunning;
  int  i;
  uint32_t usecs;
  uint32_t msecs;
  uint32_t delta;
  RTCDRV_TimerID_t id;

  UnityPrintf("6ms PERIODIC TIMER TEST\n");

  INIT_TIMESTAMP();

#ifdef TOGGLE_GPIO
  GPIO_PinModeSet(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN, gpioModePushPull, 0);
  GPIO_PinOutClear(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  /* Allocate new timer instances */
  RTCDRV_AllocateTimer( &id );

  TEST_ASSERT_EQUAL_INT(0, id);

  // Clear callback timestamps
  for (i=0; i<TIMESTAMP_TBL_SIZE; i++)
  {
    endTimestamp[i] = startTimestamp[i] = 0;
  }

  periodicCallbackCounter = 0;
  end = false;
#ifdef TOGGLE_GPIO
  GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif
  startTimestamp[0] = GET_US_TIMESTAMP;
  RTCDRV_StartTimer(id,
                    rtcdrvTimerTypePeriodic,
                    6,
                    callbackPeriodic,
                    (void*)0);

  RTCDRV_IsRunning(id, &isRunning);
  TEST_ASSERT_TRUE( isRunning );

  while(1)
  {
    if( true == end)
    {
      break;
    }
  }

  RTCDRV_IsRunning(id, &isRunning);
  TEST_ASSERT_TRUE( isRunning );

  // Stop the timer and verify that it has stopped
  RTCDRV_StopTimer(id);
  RTCDRV_IsRunning(id, &isRunning);
  TEST_ASSERT_FALSE( isRunning );

  // Verify callback timestamps
  for (i=0; i<TIMESTAMP_TBL_SIZE; i++)
  {
    msecs = 6 * (i + 1);
    delta = (msecs / 40) + 1;  // Allow for 1ms deviation per 40ms.
    usecs = endTimestamp[i] - startTimestamp[0];
    UnityPrintf("Callback %d called after %d us.\n", i+1, usecs);

    if (usecs < (msecs-delta)*1000)
    {
      UnityPrintf("Callback %d called earlier (%d us) than expected (%d us).\n",
                  i+1, usecs, msecs*1000);
      TEST_ASSERT(usecs >= (msecs-delta)*1000);
    }
    if (usecs >= (msecs+delta)*1000)
    {
      UnityPrintf("Callback %d called later (%d us) than expected (%d us).\n",
                  i+1, usecs, (msecs+delta)*1000);
      TEST_ASSERT_UINT_WITHIN(delta*1000, msecs*1000, usecs);
    }
  }

  RTCDRV_DeInit();
}


/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Test the timing of short timers.
 */

#define NUM_SHORT_TIMERS   (EMDRV_RTCDRV_NUM_TIMERS)

void callbackShortTimers(RTCDRV_TimerID_t id, void *user)
{
  (void)id;
  endTimestamp[(int)user] = GET_US_TIMESTAMP;

#ifdef TOGGLE_GPIO
  GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif

  end = true;
}

void test_timer_timing_short(void)
{
  bool isRunning;
  int  i, n;
  uint32_t usecs;
  uint32_t msecs;
  RTCDRV_TimerID_t id;

  UnityPrintf("SHORT TIMERS TEST\n");

  INIT_TIMESTAMP();

#ifdef TOGGLE_GPIO
  GPIO_PinModeSet(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN, gpioModePushPull, 0);
  GPIO_PinOutClear(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  /* Allocate new timer instances */
  RTCDRV_AllocateTimer( &id );

  TEST_ASSERT_EQUAL_INT(0, id);

  for (i=0; i<NUM_SHORT_TIMERS; i++)
  {
    for (n=0; n<10; n++)
    {
      msecs = NUM_SHORT_TIMERS - i;
      end = false;
#ifdef TOGGLE_GPIO
      GPIO_PinOutToggle(TIMING_SIGNAL_GPIO_PORT, TIMING_SIGNAL_GPIO_PIN);
#endif
      startTimestamp[i] = GET_US_TIMESTAMP;
      RTCDRV_StartTimer(id,
                        rtcdrvTimerTypeOneshot,
                        msecs,
                        callbackShortTimers,
                        (void*)i);

      RTCDRV_IsRunning(id, &isRunning);
      TEST_ASSERT_TRUE( isRunning );

      while (!end)
        ;

      RTCDRV_IsRunning(id, &isRunning);
      TEST_ASSERT_FALSE( isRunning );

      usecs = endTimestamp[i] - startTimestamp[i];

      UnityPrintf("Timer %d of %d ms elapsed %d us.\n", i+1, msecs, usecs);

      uint32_t usecMin = ((msecs*1000) - RTC_TICK_IN_USEC);
      uint32_t usecMax = ((msecs*1000) + (2*RTC_TICK_IN_USEC));

      if (usecs < usecMin)
      {
        UnityPrintf("Timer %d ended earlier (%d us) than expected (%d us) min limit (%d us).\n",
                    i, usecs, msecs*1000, usecMin);
        TEST_ASSERT(usecs > usecMin);
      }
      if (usecs >= usecMax)
      {
        UnityPrintf("Timer %d ended later (%d us) than expected (%d us) max limit (%d us).\n",
                    i, usecs, msecs*1000, usecMax);
        TEST_ASSERT(usecs < usecMax);
      }
    }
  }

  RTCDRV_DeInit();
}


#if defined ( LETIMER_PRESENT ) && defined( LETIMER_CTRL_RTCC0TEN )

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer0 for 1000ms
 * 2. Set Timer1 for 300ms
 * 3. in Timeout callback from Timer1 validate time and set Timer1 to 800ms
 * 4. Get timeout for Timer0 and validate time.
 * 5. Get timeout for Timer1, validate time and set end of this test case.
 */

void callbackTiming0(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);
  setupTimer(1,800,callbackTimingLast);
}


/* Third scenario callbacks */
void test_timer_timing(void)
{
  INIT_TIMESTAMP();

  /* Enable clock for LETIMER module */
  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  setupTimer(0,1000,callbackTimingBasic);
  setupTimer(1,300,callbackTiming0);

  while(1)
   {
     /* Check if was interrupt from RTC */
      if( true == end)
     {
       break;
     }
   }
  RTCDRV_DeInit();
}


/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer0 for 1000ms
 * 2. Set Timer1 for 300ms
 * 3. Set Timer2 for 1500ms
 * 4. in Timeout callback from Timer1 validate time and stop Timer0. Setup LETIMER
 *    to trigger interrupt in 400ms.
 * 5. Get LETIMER interrupt and setup Timer0 for 1000ms.
 * 6. Get timeout for Timer2 and validate time.
 * 7. Get timeout for Timer0 and validate time.
 */
void setupLETIMERint(uint32_t timeout)
{
  timeout *= 32.768;
  uint32_t cur = LETIMER_CounterGet(LETIMER0);
  uint32_t comp = cur - timeout;
  LETIMER_CompareSet(LETIMER0,0,comp);
  LETIMER_IntClear(LETIMER0,1);
  LETIMER_IntEnable(LETIMER0, 0x1);
  NVIC_ClearPendingIRQ(LETIMER0_IRQn);
  NVIC_EnableIRQ(LETIMER0_IRQn);
}

void LETIMER0_IRQHandler(void)
{
  LETIMER_IntClear(LETIMER0,1);
  LETIMER_IntDisable(LETIMER0, 0x1);

  UnityPrint("LETIMER0_IRQHandler");
  UNITY_PRINT_EOL;

  if(letimer_Userclbk) (*letimer_Userclbk)();

  if(letimer_clbk)
  {
    setupTimer(letimer_id,letimer_irqTimeout,letimer_clbk);
  }
}

void callbackTiming1(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);
  RTCDRV_StopTimer(id0);
  setupLETIMERint(400);
}


/* Third scenario callbacks */
void test_timer_timing1(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;


    end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  letimer_irqTimeout = 1000;
  letimer_id = 0;
  letimer_clbk = callbackTimingLast;
  letimer_Userclbk = 0;

  setupTimer(0,1000,callbackTimingBasic);
  setupTimer(1,300,callbackTiming1);
  setupTimer(2,1500,callbackTimingBasic);


  while(1)
   {
     /* Check if was interrupt from RTC */
      if( true == end)
     {
       break;
     }
   }
  RTCDRV_DeInit();
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer1 for 300ms
 * 2. Set Timer2 for 400ms
 * 3. in Timeout callback from Timer1 validate time and set Timer0 to 200ms
 * 5. Get timeout for Timer2 and validate time
 * 6. Get timeout for Timer0 and validate time and set end of this test case
 */
void callbackTiming2(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);
  setupTimer(0,200,callbackTimingLast);
}

/* Third scenario callbacks */
void test_timer_timing2(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  setupTimer(1,300,callbackTiming2);
  setupTimer(2,400,callbackTimingBasic);

    while(1)
   {
     /* Check if was interrupt from RTC */
      if( true == end)
     {
       break;
     }
   }
  RTCDRV_DeInit();
}


/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer0 for 1s
 * 2. Set Timer1 for 300ms
 * 3. in Timeout callback from Timer1 validate time and set Timer2 to 400ms
 *    and LETIMER to trigger interrupt in 100ms
 * 5. In LETIMER interrupt setup Timer1 to 100ms
 * 6. in Timeout callback from Timer1 validate timing
 * 7. In Timeout callback from Timer0 validate timing and set end of this test case
 */
void callbackTiming3(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);
  setupTimer(2,400,callbackTimingBasic);

  letimer_irqTimeout = 100;
  letimer_id = 1;
  letimer_clbk = callbackTimingBasic;

  setupLETIMERint(100);
}

/* Third scenario callbacks */
void test_timer_timing3(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

   end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  setupSecTimer(0,1,callbackTimingLast);
  setupTimer(1,300,callbackTiming3);

   while(1)
   {
     /* Check if was interrupt from RTC */
    if( true == end)
    {
       break;
    }
  }
  RTCDRV_DeInit();
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer2 for 100ms
 * 2. in Timeout callback from Timer2 validate time and set Timer0 to 100ms
 *    Timer1 to 50ms and Timer2 to 20ms
 * 3. Get timeout for Timer2 and validate time
 * 4. Get timeout for Timer1 and validate time
 * 5. Get timeout for Timer0 and validate time and set end of this test case
 */

void callbackTiming4(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);
  setupTimer(0,100,callbackTimingLast);
  setupTimer(1,50,callbackTimingBasic);
  setupTimer(2,20,callbackTimingBasic);
}

/* Third scenario callbacks */
void test_timer_timing4(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();
  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  setupTimer(2,100,callbackTiming4);

  while(1)
  {
    /* Check if was interrupt from RTC */
    if( true == end)
    {
      break;
    }
  }
  RTCDRV_DeInit();
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer2 for 100ms
 * 2. in Timeout callback from Timer2 validate time and set Timer0 to 20ms
 *    Timer1 to 50ms and Timer2 to 100ms
 * 3. Get timeout for Timer2 and validate time
 * 4. Get timeout for Timer1 and validate time
 * 5. Get timeout for Timer0 and validate time
 */

void callbackTiming5(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);
  setupTimer(0,20,callbackTimingLast);
  setupTimer(1,50,callbackTimingBasic);
  setupTimer(2,100,callbackTimingBasic);
}

/* Third scenario callbacks */
void test_timer_timing5(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  setupTimer(2,100,callbackTiming4);

  while(1)
  {
    /* Check if was interrupt from RTC */
    if( true == end)
    {
      break;
    }
  }
  RTCDRV_DeInit();
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer0 for 50ms periodic.
 * 2. Setup LETIMER to get interrupt in 20ms.
 * 3. In LETIMER setup timer1 for 20ms.
 * 4. Get Timer1 callback and validate time.
 * 5. in Timeout callback from Timer0 validate time and decrement periodicRpt.
 *    a) Setup LETIMER to get interrupt in 20ms
 *    b)If periodic timer is repeated given number of times stop it.Set Timer0 for 20ms.
 * 6. in every LETIMER interrupt setup timer1 for 20ms
 * 7. Get Timer1 callback and validate time.
 * 8. Get timeout for Timer0 and validate time.
 */
uint32_t periodicRpt = 10;
void callbackTiming6(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);

  timerUt[id].timestamp = GET_MS_TIMESTAMP;
  periodicRpt--;
  if(periodicRpt == 0)
  {
    RTCDRV_StopTimer(id);
    setupTimer(0,20,callbackTimingLast);
  }
  else
  {
    setupLETIMERint(20);
  }
}

/* Third scenario callbacks */
void test_timer_timing6(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  letimer_irqTimeout = 20;
  letimer_id = 1;
  letimer_clbk = callbackTimingBasic;

  setupPeriodicTimer(0,50,callbackTiming6);
  setupLETIMERint(20);

  while(1)
  {
    /* Check if was interrupt from RTC */
    if( true == end)
    {
      break;
    }
  }
  RTCDRV_DeInit();
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer0 for 50ms
 * 2. Set Timer1 for 20ms
 * 3. Get Timer1 callback and validate time. Setup Timer0 for 20ms
 * 4. Get Timer0 callback and validate time and set end of this test case
 */
void callbackTiming7(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);
  setupTimer(0,20,callbackTimingLast);
}

/* Third scenario callbacks */
void test_timer_timing7(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  setupTimer(0,50,callbackTimingLast);
  setupTimer(1,20,callbackTiming7);
  while(1)
  {
    /* Check if was interrupt from RTC */
    if( true == end)
    {
      break;
    }
  }
  RTCDRV_DeInit();
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer0 for 3 seconds
 * 2. in Timeout callback from Timer0 validate time
 *    and check if RTCDRV_TimerSleepReadyGet() return 1
 *    and set end of this test case
 */

void test_timer_timing8(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  setupSecTimer(0,3,callbackTimingLast);
  while(1)
  {
    /* Check if was interrupt from RTC */
    if( true == end)
    {
      break;
    }
  }
  RTCDRV_DeInit();
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer0 for 100ms
 * 2. Set LETIMER for interrupt in 95ms
 * 3. In LETIMER interrupt block for 40ms
 * 3. Get Timer0 callback and validate time is 135ms
 *    and set end of this test case
 */

void callback9a()
{
  uint32_t timeout = 40;
  timeout *= 32.768;
  uint32_t cur = LETIMER_CounterGet(LETIMER0);
  uint32_t comp = cur - timeout;
  LETIMER_CompareSet(LETIMER0,0,comp);
  while( !(LETIMER0->IF & LETIMER_IF_COMP0));
}

void callback9b(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  timerUt[id].timeout = 135;
  validateTiming(id);
}

void test_timer_timing9(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  setupTimer(0,100,callback9b);
  letimer_irqTimeout = 50;
  letimer_id = 1;
  letimer_clbk = callbackTimingLast;
  letimer_Userclbk = &callback9a;

  setupLETIMERint(95);
  while(1)
  {
    /* Check if was interrupt from RTC */
    if( true == end)
    {
      break;
    }
  }

  letimer_Userclbk = NULL;
  RTCDRV_DeInit();
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer0 for periodic 50ms
 * 2. Count callbacks and if reach 5th stop the timer
 * 3. Set timer0 to 20ms
 * 4. Validate timing in callback and set end of this test case
 */
static uint32_t clbk10Cnt = 5;
void callback10(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);

  timerUt[id].timestamp = GET_MS_TIMESTAMP;
  clbk10Cnt--;
  if(clbk10Cnt == 0)
  {
    RTCDRV_StopTimer(id);
    setupTimer(0,20,callbackTimingLast);
  }
}

void test_timer_timing10(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  setupPeriodicTimer(0,50,callback10);
  while(1)
  {
    /* Check if was interrupt from RTC */
    if( true == end)
    {
      break;
    }
  }

  letimer_Userclbk = NULL;
  RTCDRV_DeInit();
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/

/* Scenario:
 * 1. Set Maximum second timeout
 * 2. Stop timer. Validate that no assert was hit
 * 3. Set Maximum milisecond timeout
 * 4. Stop timer. Validate that no assert was hit
 */
#if defined (_EFM_DEVICE)
#define COMPARE_MAX_VAL   _RTC_COMP0_MASK
#elif defined (_EFR_DEVICE)
#define COMPARE_MAX_VAL   _RTCC_CNT_MASK
#endif
#if (RTCDRV_32BIT_SEC_TIMER_ENABLED == TRUE)
#define MAX_SEC           (0xFFFFFFFF)
#else
#define MAX_SEC           (COMPARE_MAX_VAL/4096)
#endif
/* Maximum possible miliseconds requested in single timer request. */
#define MAX_MSEC          (COMPARE_MAX_VAL/4096)


void test_timer_timing11(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  setupPeriodicTimer(0,MAX_MSEC-1,callback10);
  RTCDRV_StopTimer(0);

  setupSecTimer(0,MAX_SEC-1,callback10);
  RTCDRV_StopTimer(0);

  /* just want to check if code gets here and not stuck in assert.*/
  TEST_ASSERT_EQUAL(1,1);

  RTCDRV_DeInit();
}


/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer to 50ms
 * 2. After 15ms verify timeLeftGet
 * 3. Block interrupts so that timer callback is delayed
 * and verify that timeLeftGet wont return stupid values
 */
void callback12a(void)
{
  uint32_t timeLeft;
  RTCDRV_TimeRemaining(0, &timeLeft );
  TEST_ASSERT_UINT_WITHIN(2,35, timeLeft);

  letimer_irqTimeout = 0;
  letimer_id = 0;
  letimer_clbk = 0;
  letimer_Userclbk = 0;

  setupLETIMERint(40);
  CORE_ATOMIC_SECTION(
    while(0 == (LETIMER0->IF & LETIMER_IF_COMP0));

    RTCDRV_TimeRemaining(0, &timeLeft );
    TEST_ASSERT_EQUAL(0,timeLeft);
  )

}

void callback12b(RTCDRV_TimerID_t id, void *user)
{
  (void)id;
  (void)user;

#if defined( EMDRV_RTCDRV_SLEEPDRV_INTEGRATION )
  #if defined( _EFR_DEVICE )
    // Lowest sleep mode shall be EM3
    TEST_ASSERT_EQUAL( sleepEM3, SLEEP_LowestEnergyModeGet() );
  #else
    #if defined( EMDRV_RTCDRV_WALLCLOCK_CONFIG )
      // Lowest sleep mode shall be EM2
      TEST_ASSERT_EQUAL( sleepEM2, SLEEP_LowestEnergyModeGet() );
    #else
      // Lowest sleep mode shall be EM3
      TEST_ASSERT_EQUAL( sleepEM3, SLEEP_LowestEnergyModeGet() );
    #endif
  #endif
#endif

  end = true;
}
void test_timer_timing12(void)
{
  INIT_TIMESTAMP();

  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  RTCDRV_AllocateTimer( &id2 );

  setupTimer(0,50,callback12b);
  letimer_irqTimeout = 0;
  letimer_id = 0;
  letimer_clbk = 0;
  letimer_Userclbk = &callback12a;

  setupLETIMERint(15);

  while(1)
   {
     /* Check if was interrupt from RTC */
    if( true == end)
     {
       break;
     }
   }

  /* just want to check if code gets here and not stuck in assert.*/
  TEST_ASSERT_EQUAL(1,1);

  RTCDRV_DeInit();
}
/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set new time (WallClock)
 * 2. Set Timer0 to 5s.
 * 2. in Timeout callback from Timer0 validate time and check WallClock timing
 *
 */
void callback11a(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTimingUnprecise(id, 100);
  UnityPrint("Actual time:");
  UnityPrintNumberUnsigned(RTCDRV_GetWallClock());
  UnityPrint(" seconds");
  UNITY_PRINT_EOL;
  TEST_ASSERT_EQUAL(setTime +5,RTCDRV_GetWallClock());
  end = true;
}

void callback11b(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);
}

void test_wall_clock(void)
{
  INIT_TIMESTAMP();

  /* Enable clock for LETIMER module */
  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

   end = false;

  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );

  setupTimer(1,200,callback11b);
  RTCDRV_SetWallClock(setTime);
  setupSecTimer(0,5,callback11a);

  while(1)
   {
     /* Check if was interrupt from RTC */
    if( true == end)
     {
       break;
     }
   }

  /* just want to check if code gets here and not stuck in assert.*/
  TEST_ASSERT_EQUAL(1,1);
  RTCDRV_DeInit();
}


/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Set Timer to 150ms.
 * 2. Set blocking delay to 2 s.
 * 3. in Timeout callback from Timer0 validate time
 * 4. Check blocking delay timing
 */
void callback13(RTCDRV_TimerID_t id, void *user)
{
  (void)user;

  validateTiming(id);
  end = true;
}

void test_timer_blocking_delay(void)
{
  INIT_TIMESTAMP();

  /* Enable clock for LETIMER module */
  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;
  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  setupTimer(0,150,callback13);

  timerUt[1].timeout = 2000;
  timerUt[1].timestamp = GET_MS_TIMESTAMP;
  RTCDRV_Delay (2000);
  validateTiming(1);
  while(1)
   {
     /* Check if was interrupt from RTC */
    if( true == end )
     {
       break;
     }
   }

  /* just want to check if code gets here and not stuck in assert.*/
  TEST_ASSERT_EQUAL(1,1);
  RTCDRV_DeInit();
}


/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/
/* Scenario:
 * 1. Timer0 set for 200ms
 * 2. Timer1 set for 800 ms
 * 3. Stop Timer0
 * 4. in Timeout callback from Timer1 validate time
 * 4. Check if Timer0 didn't fire
*/
void callback14a(RTCDRV_TimerID_t id, void *user)
{
  (void)id;
  (void)user;
  execute = true;
}
void callback14b(RTCDRV_TimerID_t id, void *user)
{
  (void)user;
  validateTiming(id);
  end = true;
}

void test_stop_first_running(void)
{
  INIT_TIMESTAMP();

 /* Enable clock for LETIMER module */
  LETIMER_Init_TypeDef init = LETIMER_INIT_DEFAULT;
  init.rtcComp0Enable = 1;

  end = false;
  /* Initialization of RTCDRV */
  RTCDRV_DeInit();
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_LETIMER0, true);
  LETIMER_Init(LETIMER0, &init);

  RTCDRV_AllocateTimer( &id0 );
  RTCDRV_AllocateTimer( &id1 );
  setupTimer(0,200,callback14a);
  setupTimer(1,800,callback14b);

  RTCDRV_StopTimer(id0);
  while(1)
   {
     /* Check if was interrupt from RTC */
    if( true == end )
     {
        TEST_ASSERT_EQUAL(false,execute);
       break;
     }
   }

  /* just want to check if code gets here and not stuck in assert.*/
  TEST_ASSERT_EQUAL(1,1);
  RTCDRV_DeInit();
}

#endif /* defined(LETIMER_PRESENT) && defined(LETIMER_CTRL_RTCC0TEN) */


void main_test_RTCDRV(void)
{
  UnityBeginGroup("unity_RTCDRV");

#if defined( EMDRV_RTCDRV_SLEEPDRV_INTEGRATION )
  SLEEP_Init( NULL, NULL );
#endif

  RUN_TEST(test_timer_1ms_periodic, __LINE__);
  RUN_TEST(test_timer_6ms_periodic, __LINE__);
  RUN_TEST(test_timer_1ms_callback_restart, __LINE__);
  RUN_TEST(test_timer_timing_short, __LINE__);
  RUN_TEST(test_concurrent_timers, __LINE__);

  RUN_TEST(test_RTCDRV, __LINE__);
  RUN_TEST(test_time_remaining, __LINE__);

#if defined (LETIMER_PRESENT) && defined(LETIMER_CTRL_RTCC0TEN)

  RUN_TEST(test_wall_clock, __LINE__);
  RUN_TEST(test_timer_timing, __LINE__);
  RUN_TEST(test_timer_timing1, __LINE__);
  RUN_TEST(test_timer_timing2, __LINE__);
  RUN_TEST(test_timer_timing3, __LINE__);
  RUN_TEST(test_timer_timing4, __LINE__);
  RUN_TEST(test_timer_timing5, __LINE__);
  RUN_TEST(test_timer_timing6, __LINE__);
  RUN_TEST(test_timer_timing7, __LINE__);
  RUN_TEST(test_timer_timing8, __LINE__);
  RUN_TEST(test_timer_timing9, __LINE__);
  RUN_TEST(test_timer_timing10, __LINE__);
  RUN_TEST(test_timer_timing11, __LINE__);
  RUN_TEST(test_timer_timing12, __LINE__);
  RUN_TEST(test_timer_blocking_delay, __LINE__);
  RUN_TEST(test_stop_first_running, __LINE__);
#endif /* defined(LETIMER_PRESENT) && defined(LETIMER_CTRL_RTCC0TEN) */

  //This does not work as RTCDRV is reinitialized in each test above.
  //UnityPrint("Test last: ");
  //UnityPrintNumberUnsigned(RTCDRV_GetWallClock() - setTime);
  //UnityPrint(" seconds");
  //UNITY_PRINT_EOL;

#if 0
  // Simple manual test of wallclock, run overnight !
  for (;;) {
    char *p;
    uint32_t now;

    now = RTCDRV_GetWallClock();
    p = ctime( (const time_t*)&now );
    UnityPrint( p );
    UNITY_PRINT_EOL;

    RTCDRV_Delay( 10000 );
  }
#endif
  UnityEnd();
}
