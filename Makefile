BUILDDIR := $(PWD)
LIBDIR := $(BUILDDIR)/libc

PATH := /usr/local/cross/bin:$(PATH)
DIRS := kernel libc init drivers
TARGET := i586-elf

AS := nasm
CC := i586-elf-gcc
LD := i586-elf-ld
AR := i586-elf-ar

ASFLAGS := -f elf
CCFLAGS := -nostdlib -nostdinc -fno-builtin -fno-exceptions -m32 
CCFLAGS += -fomit-frame-pointer -fno-asynchronous-unwind-tables -fno-unwind-tables -I$(LIBDIR)/include
LDFLAGS := -T $(LIBDIR)/include/Link.ld
ARFLAGS := -rc

LIBC := $(LIBDIR)/libc.a

export BUILDDIR LIBDIR AS CC LD AR ASFLAGS CCFLAGS LDFLAGS ARFLAGS LIBC

.SILENT:
	
.default: all floppy emul	
	
l: all floppy emul
	
all: $(DIRS)
	
$(DIRS):	force
	@echo "  MAKE    " $@
	@cd $@; $(MAKE) $(MFLAGS)

clean:
	@for DIR in $(DIRS); do echo "  CLEAN   " $$DIR; cd $(BUILDDIR)/$$DIR; make clean; done;

floppy: force
	@echo "  UPDATING IMAGE"
	@build/update_image.sh

emul: force
	@echo "  STARTING EMULATOR"
	@build/emul.sh

force:
	true
