#ifndef __CORTEX_CONFIG_H__
#define __CORTEX_CONFIG_H__

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

#if 0
#define CONFIG_GUI 0
#define CONFIG_SMALL 1
#define CONFIG_INIT_ARRAY 0
#else
#define CONFIG_GUI 1
#define CONFIG_SMALL 0
#define CONFIG_INIT_ARRAY 1
#endif

#define CONFIG_QEMU_SEMIHOSTING 0

#endif
