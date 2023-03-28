#define CONFIG_SMP 1
#define CONFIG_DEBUG 1

#if CONFIG_DEBUG
#define CONFIG_STACK_INIT 1
#else
#define CONFIG_STACK_INIT 0
#endif
