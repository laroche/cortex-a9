#ifndef __STARTUP_H__
#define __STARTUP_H__

__attribute__ ((section(".isr_vector"),naked,used)) void VectorTable(void);
__attribute__ ((naked,noreturn)) void LoopHandler(void);
int main(void);
/* __attribute__ ((noreturn)) */ void qemu_exit(void);

#endif
