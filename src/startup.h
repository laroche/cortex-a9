#ifndef __STARTUP_H__
#define __STARTUP_H__

#include <sys/cdefs.h>

__attribute__ ((section(".isr_vector"),naked,used)) void VectorTable(void);
__attribute__ ((naked,noreturn)) void LoopHandler(void);
int main(void);
/* __dead2 */ void qemu_exit(void);

#endif
