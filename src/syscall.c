#include <errno.h>
#include <stdlib.h>
#include "pl011.h"

int _write(int f __unused, char *ptr, int len)
{
  int i;

  (void) f;
  for (i = 0; i < len; i++)
  {
    uart_putc(*ptr++);
  }
  return len;
}

/* Start of the heap.  */
extern const char _heap_start __attribute__((section(".heap")));
/* End of the heap (maximum value of heap_ptr).  */
extern const char _heap_end __attribute__((section(".heap")));

/* Extend heap space by size bytes.
   Return start of new space allocated, or -1 for errors
   Error cases:
    1. Allocation is not within heap range */
void * _sbrk (ptrdiff_t size)
{
  static const char * heap_ptr;  /* pointer to head of heap (from linker script file */
  const char * old_heap_ptr;

  /* heap_ptr is initialized to _heap_start */
  if (heap_ptr == 0)
  {
    heap_ptr = &_heap_start;
  }

  old_heap_ptr = heap_ptr;

  if ((heap_ptr + size) > &_heap_end)
  {
    /* top of heap is bigger than _heap_end */
    errno = ENOMEM;
    return (void *) -1;
  }

  /* success: update heap_ptr and return previous value */
  heap_ptr += size;
  return (void *)old_heap_ptr;
}
