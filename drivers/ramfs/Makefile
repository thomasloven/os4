TARGET := ramfs.elf
SUBDIR := drivers/ramfs

SOURCES := $(patsubst %.s,%.o,$(shell find . -name "*.s"))
SOURCES += $(patsubst %.c,%.o,$(shell find . -name "*.c"))

.SUFFIXES:
.SUFFIXES: .o .s .c
	
all: $(TARGET)
	
$(TARGET): $(SOURCES)
	@echo "   ln      " $(TARGET)
	@$(LD) $(LDFLAGS) -o $(TARGET) $(SOURCES) $(LIBC)
	
.c.o:
	@echo "   gcc     " $<
	@$(CC) $(CCFLAGS) -c $< -o $@
	
.s.o:
	@echo "   nasm    " $<
	@$(AS) $(ASFLAGS) $< -o $@
	
clean:
	-@rm $(SOURCES) 2>/dev/null
	-@rm $(TARGET) 2>/dev/null
