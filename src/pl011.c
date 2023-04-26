#include "pl011.h"

void uart_puts (const char *s)
{
	for (; *s; s++) {
		uart_putc(*s);
	}
}

static char chars[16] = "0123456789abcdef";

void uart_print_dec (unsigned long x)
{
	char *p, buf[32];

	p = buf + sizeof (buf);
	*--p = '\0';
	do {
		*--p = chars[x % 10];
		x /= 10;
	} while (x != 0UL);
	uart_puts (p);
}

void uart_print_hex (unsigned long x)
{
	char *p, buf[32];

	p = buf + sizeof (buf);
	*--p = '\0';
	do {
		*--p = chars[x % 16];
		x /= 16;
	} while (x != 0UL);
	uart_puts (p);
}
