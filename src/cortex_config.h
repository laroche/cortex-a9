#define CONFIG_SMP 1
/* XXX #if __VFP_FP__ */
#ifdef __ARM_FP
#define CONFIG_ARM_NEON 1
#else
#define CONFIG_ARM_NEON 0
#endif
#define CONFIG_DEBUG 1

#if CONFIG_DEBUG
#define CONFIG_STACK_INIT 1
#else
#define CONFIG_STACK_INIT 0
#endif

#define CONFIG_ARM_ERRATA_775420 0
