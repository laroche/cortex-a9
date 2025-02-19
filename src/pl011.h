#ifndef PL011_H
#define PL011_H

#include <sys/cdefs.h>
#include <stdint.h>
#include "cortex_config.h"

typedef volatile struct {
	uint32_t DR;
	uint32_t RSR_ECR;
	uint8_t reserved1[0x10];
	uint32_t FR;
	uint8_t reserved2[0x4];
	uint32_t LPR;
	uint32_t IBRD;
	uint32_t FBRD;
	uint32_t LCR_H;
	uint32_t CR;
	uint32_t IFLS;
	uint32_t IMSC;
	uint32_t RIS;
	uint32_t MIS;
	uint32_t ICR;
	uint32_t DMACR;
} pl011_t;

#define UART0_BASE 0x10009000U

static __always_inline void uart_putc (char c)
{
	pl011_t * const UART0 = (pl011_t * const) UART0_BASE;

	while (UART0->FR & 0x20U)
		;
	UART0->DR = c;
}

void uart_puts(const char *s);
__no_stackprot __attribute__ ((format(printf, 1, 2))) void uart_printf(const char *fmt, ...);

#endif
