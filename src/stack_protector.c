#include <string.h>
#include <unistd.h>

uintptr_t __stack_chk_guard = 0x00000aff; /* 0, 0, '\n', 255 */

__attribute__((noreturn)) void __stack_chk_fail (void)
{
  const char msg[] = "*** stack smashing detected ***: terminated\n";
  write(2, msg, strlen(msg));
  _exit(127);
}

#ifdef __ELF__
__attribute__((visibility ("hidden"))) void __stack_chk_fail_local (void)
{
  __stack_chk_fail();
}
#endif
