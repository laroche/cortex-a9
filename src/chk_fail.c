#include <sys/cdefs.h>
#include <unistd.h>
#include "pl011.h"

#ifdef __SPARSE__
#define __dead2 __attribute__ ((noreturn))
#endif

__dead2 void __chk_fail (void)
{
  uart_printf("*** buffer overflow detected ***: terminated\n");
  _exit(127);
}
