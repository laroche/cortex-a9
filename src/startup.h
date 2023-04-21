#ifndef __STARTUP_H__
#define __STARTUP_H__

int main (void);
void __attribute__ ((section(".isr_vector"),naked,used)) VectorTable (void);

#endif
