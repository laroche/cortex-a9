SRCDIR = src
OBJDIR = obj
BINDIR = bin

KERNEL = $(BINDIR)/kernel.elf
CORE = cortex-a9

TOOLCHAIN = arm-none-eabi-
CC = $(TOOLCHAIN)gcc
AS = $(TOOLCHAIN)as
SIZE = $(TOOLCHAIN)size
DUMP = $(TOOLCHAIN)objdump
GDB = $(TOOLCHAIN)gdb

OPTS     = --specs=nano.specs --specs=nosys.specs -nostartfiles
CFLAGS   = -mcpu=$(CORE) -O2 -Wall -Wextra -pedantic $(OPTS) -g
#CFLAGS += -Wundef -Wshadow -Wwrite-strings -Wold-style-definition -Wcast-align=strict -Wunreachable-code -Waggregate-return -Wlogical-op -Wtrampolines -Wc90-c99-compat -Wc99-c11-compat
#CFLAGS += -Wconversion -Wmissing-prototypes -Wredundant-decls -Wnested-externs -Wcast-qual -Wswitch-default
CFLAGS  += -MMD -MP
AFLAGS   = --warn
LFLAGS   = -mcpu=$(CORE) -T $(SRCDIR)/linker.ld $(OPTS) -g

CFLAGS += -ffunction-sections
LFLAGS += -Wl,--gc-sections
# For debugging:
#LFLAGS += -Wl,--print-gc-sections

#CFLAGS += -flto
#LFLAGS += -flto

QEMU = qemu-system-arm
QEMU_OPTS = -M vexpress-a9 -serial mon:stdio

C_FILES := $(wildcard $(SRCDIR)/*.c)
AS_FILES := $(wildcard $(SRCDIR)/*.S)
OBJECTS_C := $(addprefix $(OBJDIR)/,$(notdir $(C_FILES:.c=.o)))
OBJECTS_S := $(addprefix $(OBJDIR)/,$(notdir $(AS_FILES:.S=.o)))
OBJECTS_ALL := $(OBJECTS_S) $(OBJECTS_C)

$(KERNEL): $(OBJECTS_ALL)
	@mkdir -p $(@D)
	$(CC) $(LFLAGS) $(OBJECTS_ALL) -o $@ -Wl,-Map=$(BINDIR)/kernel.map
	$(DUMP) -d $@ > $(BINDIR)/kernel.lst
	@$(SIZE) $@

-include $(wildcard $(OBJDIR)/$*.d)

$(OBJECTS_ALL) : | obj

$(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o : $(SRCDIR)/%.S
	$(AS) $(AFLAGS) $< -o $@

.PHONY: qemu dqemu gdb clean

qemu: $(KERNEL)
	QEMU_AUDIO_DRV=none $(QEMU) $(QEMU_OPTS) -kernel $(KERNEL)

dqemu: $(KERNEL)
	QEMU_AUDIO_DRV=none $(QEMU) -s -S $(QEMU_OPTS) -kernel $(KERNEL)

gdb:
	$(GDB) $(KERNEL)

clean:
	rm -fr $(OBJDIR) $(BINDIR)

