TARGET := kernel
SUBDIR := kernel

SOURCES := kinit.o boot.o
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s" | grep -v boot.s))
SOURCES += $(patsubst %.c,%.o,$(shell find . -name "*.c" | grep -v kinit.c))

#CCFLAGS := -nostdlib -nostdinc -fno-builtin -m32 -I./include -fno-exceptions
CCFLAGS += -Iinclude
LDFLAGS := -T ./include/Link.ld

.SUFFIXES:
.SUFFIXES: .o .s .c
	
all: $(TARGET)
	
$(TARGET): $(SOURCES)
	@echo "   ln      " $(TARGET)
	@$(LD) $(LDFLAGS) -o $(TARGET) $(SOURCES)
	
.c.o:
	@echo "   gcc     " $<
	@$(CC) $(CCFLAGS) -c $< -o $@
	
.s.o:
	@echo "   nasm    " $<
	@$(AS) $(ASFLAGS) $< -o $@
	
clean:
	-@rm $(SOURCES) 2>/dev/null
	-@rm $(TARGET) 2>/dev/null
