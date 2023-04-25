#include "pl011.h"

void uart_puts (const char *s)
{
	for (; *s; s++) {
		uart_putc(*s);
	}
}

/* Print X in base BASE.  */
void uart_print_number (unsigned int base, unsigned long x)
{
	static char chars[16] = "0123456789abcdef";
	char *p, buf[32];

	p = buf + sizeof (buf);
	*--p = '\0';
	do {
		*--p = chars[x % base];
		x /= base;
	} while (x != 0UL);
	uart_puts (p);
}

