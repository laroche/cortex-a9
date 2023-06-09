# cortex-a9

This is my attempt to do baremetal programming on cortex-a9 using qemu and vexpress-a9
so far I got keyboard (pl050), CLCD display (pl111), timer (sp804), uart (pl011), interrupt (bare minimum Generic Interrupt Controller) (gic) working.


## Prepare software needs

On Debian or Ubuntu you should be ready to go with:
<pre>
sudo apt install gcc-arm-none-eabi make qemu-system-arm
</pre>


## Build and run

To first compile the software and then run the qemu emulator:
<pre>
make
make run
</pre>


## TODO

- Why is no "\n" needed at the end of printf() for period timer handler?
- Merge Cortex-A15 support? Also Cortex-R5?
- Can we update the gic support to a current and supported version?
- Network driver support?
- Add FreeRTOS support.
- Update to newest uGUI release.
- Add more SMP support.
   - Check: <https://github.com/s-matyukevich/raspberry-pi-os/issues/8>
- Check out: <https://github.com/ESLab/FreeRTOS---ARM-Cortex-A9-VersatileExpress-Quad-Core-port>
- Check out: <https://github.com/mytchel/agatha/blob/master/arm/kern/vectors.S>
- All failure routines that go into an endless loop are merged together.
- We could also use __dprintf() from newlib for debug output?
- If sparse checking is enabled, it runs with normal system headers instead of using
  newlib header files.
- For embedded, you should check all invocations for _exit, abort, assert.
  Are kill/signal/raise indeed missing?
  Check/monitor all calls to malloc/free?

tags: qemu cortex-a9 vexpress-a9 arm gic pl050 pl111 sp804 pl011 kmi clcd timer uart irq
