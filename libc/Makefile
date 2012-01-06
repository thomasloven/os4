TARGET := libc.a
SUBDIR := libc


SOURCES := $(patsubst %.s,%.o,$(shell find . -name "*.s"))
SOURCES += $(patsubst %.c,%.o,$(shell find . -name "*.c"))

.SUFFIXES:
.SUFFIXES: .o .s .c
	
all: $(TARGET)
	
$(TARGET): $(SOURCES)
	@echo "   ar      " $(TARGET)
	@$(AR) $(ARFLAGS) $(TARGET) $(SOURCES)
	
.c.o:
	@echo "   gcc     " $<
	@$(CC) $(CCFLAGS) -c $< -o $@
	
.s.o:
	@echo "   nasm    " $<
	@$(AS) $(ASFLAGS) $< -o $@
	
clean:
	-@rm $(SOURCES) 2>/dev/null
	-@rm $(TARGET) 2>/dev/null
