#include <sys/cdefs.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "pl011.h"
#include "startup.h"
#include "cortex_config.h"

#if CONFIG_SMALL == 0
int _write (int f __unused, char *ptr, int len)
{
  int i;

  (void) f;

  for (i = 0; i < len; i++)
  {
    uart_putc(*ptr++);
  }
  return len;
}

/* Start and end of the heap.  */
extern char _heap_start, _heap_end;

/* Extend heap space by size bytes.
   Return start of new space allocated, or -1 for errors
   Error cases:
    1. Allocation is not within heap range */
void * _sbrk (ptrdiff_t size)
{
  static char * heap_ptr = &_heap_start;  /* pointer to head of heap (from linker script file */
  char * old_heap_ptr = heap_ptr;

  if ((heap_ptr + size) > &_heap_end)
  {
    /* top of heap is bigger than _heap_end */
    errno = ENOMEM;
    return (void *) -1;
  }

  /* success: update heap_ptr and return previous value */
  heap_ptr += size;
  return (void *) old_heap_ptr;
}

/* We disable "raise()" to reduce overall size for embedded usage.
 * kill()/signal()/raise() are removed. _exit() just loops forever. */
__dead2 void abort (void)
{
  uint32_t addr;

#ifdef __GNUC__
  /* Store instruction causing undefined exception */
  __asm__ __volatile__("sub %0, lr, #4" : "=r" (addr));
#else
  #error "Unsupported compiler."
#endif

  uart_printf("Abort called from instruction address 0x%lx.\n", addr);

#if 0
  while (1)
  {
      raise(SIGABRT);
      _exit(1);
  }
#else
  _exit(1);
#endif
}

__dead2 void _exit (int status __unused)
{
#if CONFIG_QEMU_SEMIHOSTING
  qemu_exit();
#else
  (void) status;

  LoopHandler();
#endif
}
#endif

#if CONFIG_QEMU_SEMIHOSTING
/* __dead2 */ void qemu_exit (void)
{
	__asm__ __volatile__("mov r0, #0x18");		/* angel_SWIreason_ReportException */
	/* __asm__ __volatile__("movw r1, #0x0026"); */
	/* __asm__ __volatile__("movt r1, #0x2"); */	/* ADP_Stopped_ApplicationExit */
	__asm__ __volatile__("ldr r1, =#0x20026");	/* ADP_Stopped_ApplicationExit */
	__asm__ __volatile__("svc 0x00123456");		/* semihosting call on A32 */
	/* __asm__ __volatile__("hlt #0xf000"); */
#if	0
	This line makes ARMv7M qemu resets:
	SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
#endif
}
#endif

#if CONFIG_INIT_ARRAY
void _init (void)
{
}

void _fini (void)
{
}
#endif
