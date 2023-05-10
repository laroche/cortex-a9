#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "interrupt.h"
#include "sp804.h"
#include "pl031.h"
#include "pl011.h"
#include "cortex_config.h"

/* Flags for the timer control registers  */
#define SP804_TIMER_ENABLE       (1U << 7)
#define SP804_TIMER_PERIODIC     (1U << 6)
#define SP804_TIMER_INT_ENABLE   (1U << 5)
#define SP804_TIMER_PRESCALE_1   (0U << 2)
#define SP804_TIMER_PRESCALE_16  (1U << 2)
#define SP804_TIMER_PRESCALE_256 (2U << 2)
#define SP804_TIMER_32BIT        (1U << 1)
#define SP804_TIMER_ONESHOT      (1U << 0)

typedef volatile struct {
	struct {
		uint32_t Load;
		uint32_t Value;
		uint32_t Control;
		uint32_t IntClr;
		uint32_t RIS;
		uint32_t MIS;
		uint32_t BGLoad;
		uint32_t Reserved;
	} timers[2];
} timer804_t;

#define TIMER_BASE 0x10011000U

static timer804_t * const tregs = (timer804_t *) TIMER_BASE;

volatile uint32_t counter = 0U;

static void timer_handler (void)
{
#if	CONFIG_SMALL == 0
	time_t ts = read_rtc();
	struct tm *timeinfo = localtime(&ts);

	uart_printf("counter is: %u, time: %s", counter++, asctime(timeinfo));
#else
	uart_printf("counter is: %u\n", counter++);
#endif

	tregs->timers[0].IntClr = 0U;
}

void timer_init (void)
{
	tregs->timers[0].Control = SP804_TIMER_PERIODIC | SP804_TIMER_32BIT  | SP804_TIMER_PRESCALE_256 | SP804_TIMER_INT_ENABLE;
	tregs->timers[0].Load = 0U;
	tregs->timers[0].Value = 0U;
	tregs->timers[0].BGLoad = 0x1000U;
	tregs->timers[0].Control |= SP804_TIMER_ENABLE;

	install_isr(TIM01INT_IRQn, timer_handler);
	enable_irq(TIM01INT_IRQn);
}
