#include <stddef.h>
#include "em_device.h"
#include "em_assert.h"
#include "em_cmu.h"
#include "em_timer.h"

#define USTIMESTAMP32_TIMER         (TIMER1)
#define USTIMESTAMP32_TIMER_CLOCK   (cmuClock_TIMER1)
#define USTIMESTAMP32_TIMER_IRQ     (TIMER1_IRQn)

/**
 * Timer functions for microsecond ticker.
 * mbed expects a 32-bit timer. Since the EFM32 only has 16-bit timers,
 * the upper 16 bits are implemented in software.
 */

static int ustimestamp32_inited = 0;	// Is ticker initialized yet

static volatile uint16_t ticker_cnt = 0;
static uint32_t ticker_freq_mhz = 0;

void TIMER1_IRQHandler(void)
{
  if (TIMER_IntGet(USTIMESTAMP32_TIMER) & TIMER_IF_OF) {
    ticker_cnt++;
    TIMER_IntClear(USTIMESTAMP32_TIMER, TIMER_IF_OF);
  }
}

void USTIMESTAMP32_Init(void)
{
  if (ustimestamp32_inited) {
    return;
  }
  ustimestamp32_inited = 1;
  
  /* Enable clock for TIMERs */
  CMU_ClockEnable(USTIMESTAMP32_TIMER_CLOCK, true);
  
  /* Clear TIMER counter value */
  TIMER_CounterSet(USTIMESTAMP32_TIMER, 0);
  
  /* Get frequency of clock in MHz for scaling ticks to microseconds */
  ticker_freq_mhz = CMU_ClockFreqGet(USTIMESTAMP32_TIMER_CLOCK) / 1000000;
  EFM_ASSERT(ticker_freq_mhz > 0);
  
  /*
   * Calculate maximum prescaler that gives at least 1 MHz frequency,
   * while keeping clock as an integer multiple of 1 MHz.
   * Example: 14 MHz => prescaler = 1 (i.e. DIV2), ticker_freq_mhz = 7;
   * 			24 MHz => prescaler = 3 (i.e. DIV8), ticker_freq_mhz = 3;
   * 			48 MHz => prescaler = 4 (i.e. DIV16), ticker_freq_mhz = 3;
   * Limit prescaling to maximum prescaler value, which is 10 (DIV1024).
   */
  uint32_t prescaler = 0;

  while((ticker_freq_mhz & 1) == 0 && prescaler <= 10) {
    ticker_freq_mhz = ticker_freq_mhz >> 1;
    prescaler++;
  }
  
  /* Set prescaler */
  USTIMESTAMP32_TIMER->CTRL =
    (USTIMESTAMP32_TIMER->CTRL & ~_TIMER_CTRL_PRESC_MASK)
    | (prescaler << _TIMER_CTRL_PRESC_SHIFT);

  /* Select Compare Channel parameters */
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModeCompare;
  
  /* Configure Compare Channel 0 */
  TIMER_InitCC(USTIMESTAMP32_TIMER, 0, &timerCCInit);
  
  /* Enable interrupt vector in NVIC */
  TIMER_IntEnable(USTIMESTAMP32_TIMER, TIMER_IEN_OF);
  NVIC_EnableIRQ(USTIMESTAMP32_TIMER_IRQ);
    
  /* Set top value */
  TIMER_TopSet(USTIMESTAMP32_TIMER, 0xFFFF);
  
  /* Start TIMER */
  TIMER_Enable(USTIMESTAMP32_TIMER, true);
}

uint32_t USTIMESTAMP32_Get(void)
{
  uint32_t countH_old, countH, countL;
  
  /* Avoid jumping in time by reading high bits twice */
  do {
    countH_old = ticker_cnt;
    /* If the counter overflowed while in the IRQ handler for the CC0 interrupt,
     * it hasn't had time to update ticker_cnt yet. Take this into account here. */
    if (TIMER_IntGet(USTIMESTAMP32_TIMER) & TIMER_IF_OF) {
      countH_old += 1;
    }
    countL = USTIMESTAMP32_TIMER->CNT;
    countH = ticker_cnt;
    /* If the counter overflowed while in the IRQ handler for the CC0 interrupt,
     * it hasn't had time to update ticker_cnt yet. Take this into account here. */
    if (TIMER_IntGet(USTIMESTAMP32_TIMER) & TIMER_IF_OF) {
      countH += 1;
    }
  } while (countH_old != countH);
  /* Divide by ticker_freq_mhz to get 1 MHz clock */
  return ((countH << 16) | countL) / ticker_freq_mhz;
}
