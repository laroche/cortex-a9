#ifndef STARTUP_H
#define STARTUP_H

#include <sys/cdefs.h>
#include "cortex_config.h"

__no_stackprot __attribute__ ((section(".isr_vector"),naked,used)) void VectorTable(void);
__attribute__ ((naked,noreturn)) void LoopHandler(void);
int main(void);
/* __dead2 */ void qemu_exit(void);

#endif
