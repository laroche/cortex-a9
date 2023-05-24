# Build a release or a debug version:
RELEASE = 0
# Do we want to have GUI output? Also look at src/cortex_config.h:
CONFIG_GUI = 1

SRCDIR = src
OBJDIR = obj
BINDIR = bin

KERNEL = $(BINDIR)/kernel.elf
MARCH=-march=armv7-a
# Enable the following line to add floating point support:
#MARCH=-march=armv7-a+vfpv3 -mfloat-abi=hard -Wno-attributes
# As possible addition:
#MARCH += -mtune=cortex-a9

TOOLCHAIN ?= arm-none-eabi-
CC = $(TOOLCHAIN)gcc
SIZE = $(TOOLCHAIN)size
OBJDUMP = $(TOOLCHAIN)objdump
GDB = $(TOOLCHAIN)gdb

OPTS     = --specs=nano.specs --specs=nosys.specs -nostartfiles
CFLAGS   = $(MARCH) -O2 -Wall -Wextra -pedantic $(OPTS) -g
#CFLAGS += -Wundef -Wshadow -Wwrite-strings -Wold-style-definition -Wcast-align=strict -Wunreachable-code -Waggregate-return -Wlogical-op -Wtrampolines -Wc90-c99-compat -Wc99-c11-compat
#CFLAGS += -Wconversion -Wmissing-prototypes -Wredundant-decls -Wnested-externs -Wcast-qual -Wswitch-default
CFLAGS  += -MMD -MP
LFLAGS   = $(MARCH) -T $(SRCDIR)/linker.ld $(OPTS) -g

CFLAGS += -ffunction-sections -fdata-sections
LFLAGS += -Wl,--gc-sections
# For debugging:
#LFLAGS += -Wl,--print-gc-sections

#CFLAGS += -flto
#LFLAGS += -flto

ifeq ($(RELEASE),0)
CFLAGS += -DDEBUG
CFLAGS += -fstack-protector-strong
#CFLAGS += -fstack-protector-all
CFLAGS += -D_FORTIFY_SOURCE=2
endif

QEMU = qemu-system-arm
QEMU_OPTS = -M vexpress-a9 -no-reboot -serial mon:stdio -d guest_errors,unimp
ifeq ($(CONFIG_GUI),0)
QEMU_OPTS += -nographic
endif
#QEMU_OPTS += -semihosting -semihosting-config enable=on,target=native
#QEMU_OPTS += -smp 4
#QEMU_OPTS += -d int

C_FILES := $(wildcard $(SRCDIR)/*.c)
OBJECTS_ALL := $(addprefix $(OBJDIR)/,$(notdir $(C_FILES:.c=.o)))

# Detect Windows with two possible ways. On Linux start parallel builds:
ifeq ($(OS),Windows_NT)
else
ifeq '$(findstring ;,$(PATH))' ';'
else
CORES?=$(shell (nproc --all || sysctl -n hw.ncpu) 2>/dev/null || echo 1)
ifneq ($(CORES),1)
.PHONY: _all
_all:
	$(MAKE) all -j$(CORES)
endif
endif
endif

all: $(KERNEL)

$(KERNEL): $(OBJECTS_ALL) $(SRCDIR)/linker.ld | $(OBJDIR)
	@mkdir -p $(@D)
	$(CC) $(LFLAGS) $(OBJECTS_ALL) -o $@ -Wl,-Map=$(BINDIR)/kernel.map
	$(OBJDUMP) -d $@ > $(BINDIR)/kernel.lst
	$(SIZE) $@

-include $(wildcard $(OBJDIR)/*.d)

$(OBJECTS_ALL) : Makefile | $(OBJDIR)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all qemu dqemu run gdb clean

qemu: $(KERNEL)
	QEMU_AUDIO_DRV=none $(QEMU) $(QEMU_OPTS) -kernel $(KERNEL)

dqemu: $(KERNEL)
	QEMU_AUDIO_DRV=none $(QEMU) -s -S $(QEMU_OPTS) -kernel $(KERNEL)

run: qemu

gdb: $(KERNEL)
	$(GDB) $(KERNEL)

clean:
	rm -fr $(OBJDIR) $(BINDIR)

