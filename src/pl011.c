#include <stdarg.h>
#include "pl011.h"

void uart_puts (const char *s)
{
	for (; *s; s++) {
		uart_putc(*s);
	}
}

static char chars[16] = "0123456789abcdef";

static void uart_print_dec (unsigned long x)
{
	char *p, buf[32];

	p = buf + sizeof (buf);
	*--p = '\0';
	do {
		*--p = chars[x % 10UL];
		x /= 10UL;
	} while (x != 0UL);
	uart_puts(p);
}

static void uart_print_hex (unsigned long x)
{
	char *p, buf[32];

	p = buf + sizeof (buf);
	*--p = '\0';
	do {
		*--p = chars[x % 16UL];
		x /= 16UL;
	} while (x != 0UL);
	uart_puts(p);
}

void uart_printf(const char *fmt, ...)
{
	va_list args;
#if	0
	char buf[1024];


	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	uart_puts (buf);
	va_end(args);
#else
	unsigned int l;
	const char *p;

	va_start(args, fmt);
	while (*fmt) {
		if (*fmt != '%' || *++fmt == '%') {
			uart_putc(*fmt++);
			continue;
		}
		switch (*fmt++) {
		case 'u':
			l = va_arg(args, unsigned int);
			uart_print_dec(l);
			break;
		case 'x':
			l = va_arg(args, unsigned int);
			uart_print_hex(l);
			break;
		case 's':
			p = va_arg(args, const char *);
			uart_puts(p);
			break;
		default:
#ifdef			DEBUG
			uart_puts("\nUnknown string format character: ");
			uart_putc(fmt[-1]);
			uart_putc('\n');
#endif
			break;
		}
	}
	va_end(args);
#endif
}
