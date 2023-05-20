#include <sys/cdefs.h>
#include <string.h>
#include <unistd.h>

__dead2 void __chk_fail (void)
{
  const char msg[] = "*** buffer overflow detected ***: terminated\n";
  write(2, msg, strlen(msg));
  _exit(127);
}
