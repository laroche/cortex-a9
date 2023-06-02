#include <sys/cdefs.h>
#include <string.h>
#include <unistd.h>

#ifdef __SPARSE__
#define __dead2 __attribute__ ((noreturn))
#endif

__dead2 void __chk_fail (void)
{
  const char msg[] = "*** buffer overflow detected ***: terminated\n";
  write(2, msg, strlen(msg));
  _exit(127);
}
