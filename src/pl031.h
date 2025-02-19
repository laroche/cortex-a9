#ifndef PL031_H
#define PL031_H

#include <sys/cdefs.h>
#include <stdint.h>

typedef volatile struct {
	uint32_t RTCDR;
	uint32_t RTCMR;
	uint32_t RTCLR;
	uint32_t RTCCR;
	uint32_t RTCIMSC;
	uint32_t RTCRIS;
	uint32_t RTCMIS;
	uint32_t RTCICR;
} pl031_t;

#define RTC_BASE 0x10017000

static __always_inline uint32_t read_rtc (void)
{
	pl031_t * const RTC = (pl031_t * const) RTC_BASE;

	return RTC->RTCDR;
}
#endif

