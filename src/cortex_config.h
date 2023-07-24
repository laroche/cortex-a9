#ifndef CORTEX_CONFIG_H
#define CORTEX_CONFIG_H

#define CONFIG_SMP 1
#ifdef __ARM_FP
#define CONFIG_ARM_FP 1
#else
#define CONFIG_ARM_FP 0
#endif
#define CONFIG_DEBUG 1

#if CONFIG_DEBUG
#define CONFIG_STACK_INIT 1
#else
#define CONFIG_STACK_INIT 0
#endif

#define CONFIG_ARM_ERRATA_775420 0

#if CONFIG_GUI
#define CONFIG_SMALL 0
#define CONFIG_INIT_ARRAY 1
#else
#define CONFIG_SMALL 1
#define CONFIG_INIT_ARRAY 0
#endif

#define CONFIG_QEMU_SEMIHOSTING 0

#ifndef __clang__
#define __no_stackprot __attribute__ ((__optimize__ ("-fno-stack-protector")))
#else
#define __no_stackprot
#endif

#endif
