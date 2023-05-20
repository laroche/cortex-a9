#ifndef __PL011_H__
#define __PL011_H__

#include <stdint.h>

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

static inline __attribute__ ((always_inline)) void uart_putc (char c)
{
	pl011_t * const UART0 = (pl011_t *) UART0_BASE;

	while (UART0->FR & 0x20U)
		;
	UART0->DR = c;
}

void uart_puts(const char *s);
void uart_printf(const char *fmt, ...);

#endif
