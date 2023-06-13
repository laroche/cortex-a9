#include <stdarg.h>
#include "pl011.h"
#include "cortex_config.h"

void uart_puts (const char *s)
{
	for (; *s; s++) {
		uart_putc(*s);
	}
}

static char chars[16] = "0123456789abcdef";

static __no_stackprot void uart_print_dec (unsigned long x)
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

static __no_stackprot void uart_print_hex (unsigned long x)
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

__no_stackprot __attribute__ ((format(printf, 1, 2))) void uart_printf (const char *fmt, ...)
{
	va_list args;
#if	0
	char buf[1024];


	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	uart_puts (buf);
	va_end(args);
#else
	va_start(args, fmt);
	while (*fmt) {
		if (*fmt != '%' || *++fmt == '%') {
			uart_putc(*fmt++);
			continue;
		}
		switch (*fmt++) {
		case 'l':
			switch (*fmt++) {
			case 'u':
				uart_print_dec(va_arg(args, unsigned long));
				break;
			case 'x':
				uart_print_hex(va_arg(args, unsigned long));
				break;
			default:
#ifdef				DEBUG
				uart_puts("\nUnknown string format character (after l modifier): ");
				if (fmt[-1] != '\0') {
					uart_putc(fmt[-1]);
				}
				uart_putc('\n');
#endif
				break;
			}
			break;
		case 'u':
			uart_print_dec(va_arg(args, unsigned int));
			break;
		case 'x':
			uart_print_hex(va_arg(args, unsigned int));
			break;
		case 's':
			uart_puts(va_arg(args, const char *));
			break;
		default:
#ifdef			DEBUG
			uart_puts("\nUnknown string format character: ");
			if (fmt[-1] != '\0') {
				uart_putc(fmt[-1]);
			}
			uart_putc('\n');
#endif
			break;
		}
	}
	va_end(args);
#endif
}
