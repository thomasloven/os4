TARGET = drivers.tar
SUBDIR = drivers

DIRS = vfs tarfs

.SUFFIXES:
.SUFFIXES: .o .s .c
	
all: $(TARGET)

$(TARGET): $(DIRS)
	@echo "  tar     " $(TARGET)
	@cp ./*/*.elf ./.
	tar -cf drivers.tar *.elf myfile.txt
	@rm *.elf
	
$(DIRS):	force
	@echo "   MAKE   " $@
	@cd $@; $(MAKE) $(MFLAGS)

clean:
	@for DIR in $(DIRS); do echo "  CLEAN   " $$DIR; cd $(BUILDDIR)/$(SUBDIR)/$$DIR; make clean; done;
	@rm $(TARGET)
	
force:
	true