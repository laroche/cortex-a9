#include <sys/cdefs.h>
#include <stdint.h>
#include <unistd.h>
#include "pl011.h"

uintptr_t __stack_chk_guard = 0x00000aff; /* 0, 0, '\n', 255 */

#ifdef __SPARSE__
#define __dead2 __attribute__ ((noreturn))
#endif

__dead2 void __stack_chk_fail (void)
{
  uart_printf("*** stack smashing detected ***: terminated\n");
  _exit(127);
}

#ifdef __ELF__
__hidden void __stack_chk_fail_local (void)
{
  __stack_chk_fail();
}
#endif
