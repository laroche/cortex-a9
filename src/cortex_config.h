#define CONFIG_SMP 1
#if __VFP_FP__
#define CONFIG_ARM_NEON 0
#else
#define CONFIG_ARM_NEON 0
#endif
#define CONFIG_DEBUG 1

#if CONFIG_DEBUG
#define CONFIG_STACK_INIT 1
#else
#define CONFIG_STACK_INIT 0
#endif
#define CONFIG_PROFILING 0

#define CONFIG_ISR_ASM 1
#define CONFIG_ARM_ERRATA_775420 0
