#ifndef __STARTUP_H__
#define __STARTUP_H__

void __attribute__ ((section(".isr_vector"),naked,used)) VectorTable(void);
void __attribute__ ((naked,noreturn)) LoopHandler(void);
int main(void);
void /* __attribute__ ((noreturn)) */ qemu_exit(void);

#endif
