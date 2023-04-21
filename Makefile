SRCDIR = src
OBJDIR = obj
BINDIR = bin

KERNEL = $(BINDIR)/kernel.elf
CORE = cortex-a9

TOOLCHAIN ?= arm-none-eabi-
CC = $(TOOLCHAIN)gcc
SIZE = $(TOOLCHAIN)size
OBJDUMP = $(TOOLCHAIN)objdump
GDB = $(TOOLCHAIN)gdb

OPTS     = --specs=nano.specs --specs=nosys.specs -nostartfiles
CFLAGS   = -mcpu=$(CORE) -O2 -Wall -Wextra -pedantic $(OPTS) -g
#CFLAGS += -Wundef -Wshadow -Wwrite-strings -Wold-style-definition -Wcast-align=strict -Wunreachable-code -Waggregate-return -Wlogical-op -Wtrampolines -Wc90-c99-compat -Wc99-c11-compat
#CFLAGS += -Wconversion -Wmissing-prototypes -Wredundant-decls -Wnested-externs -Wcast-qual -Wswitch-default
CFLAGS  += -MMD -MP
LFLAGS   = -mcpu=$(CORE) -T $(SRCDIR)/linker.ld $(OPTS) -g

CFLAGS += -ffunction-sections -fdata-sections
LFLAGS += -Wl,--gc-sections
# For debugging:
#LFLAGS += -Wl,--print-gc-sections

#CFLAGS += -flto
#LFLAGS += -flto

CFLAGS += -DDEBUG

# Floating Point:
#CFLAGS += -mfpu=neon

QEMU = qemu-system-arm
QEMU_OPTS = -M vexpress-a9 -smp 4 -serial mon:stdio -d guest_errors,unimp
# -d int

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

