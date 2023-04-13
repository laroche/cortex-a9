# cortex-a9

This is my attempt to do baremetal programming on cortex-a9 using qemu and vexpress-a9
so far I got keyboard (pl050), CLCD display (pl111), timer (sp804), uart (pl011), interrupt (bare minimum Generic Interrupt Controller) (gic) working.

the code is a mess at this moment.. this is just a proof of concept
if I have time I'm planning to clean up the code and seperate things like (Makefile, drivers, syscall, startup, interrupt ..etc)

## TODO

- Why is no "\n" needed at the end of printf() for period timer handler?
- Merge Cortex-A15 support?
- Update to newest uGUI release.
- Add more SMP support.
- How can we merge together all failure routines that go into an endless loop?

tags: qemu cortex-a9 vexpress-a9 arm gic pl050 pl111 sp804 pl011 kmi clcd timer uart irq
