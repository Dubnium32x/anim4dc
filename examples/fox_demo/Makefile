# Anim4DC Fox Demo Makefile
# KOS-style structure using standard kos-cc (Darc's approach!)

# Put the filename of the output binary here
TARGET = fox_demo.elf

# List all of your C files here, but change the extension to ".o"
OBJS = main.o romdisk.o

# Optional path to a directory of resources to bundle within your ELF binary.
# Its contents are accessible via the "/rd/" virtual directory at runtime.
KOS_ROMDISK_DIR = romdisk

# Additional includes for our plugin
KOS_CFLAGS += -I../../include

# Main rule
all: $(TARGET)

# Include KOS build system
include $(KOS_BASE)/Makefile.rules

# Standard compile rule handled by KOS (%.o: %.c automatically provided)

# Build the target (link everything together)
$(TARGET): $(OBJS)
	kos-cc -o $@ $(OBJS) -lraylib -lGL

# Romdisk creation (using KOS variables)
romdisk.o:
	$(KOS_GENROMFS) -f romdisk.img -d romdisk -V ROMDISK
	$(KOS_OBJCOPY) -I binary -O elf32-shl -B sh \
		--redefine-sym _binary_romdisk_img_start=__romdisk_img \
		--redefine-sym _binary_romdisk_img_end=__romdisk_img_end \
		romdisk.img romdisk.o

# CDI creation for Dreamcast hardware/emulator
cdi: $(TARGET)
	@echo "Creating Anim4DC Fox Demo CDI"
	@if [ -n "$(MKDCDISC)" ]; then \
		$(MKDCDISC) -e $(TARGET) -o fox_demo.cdi; \
	elif command -v mkdcdisc >/dev/null 2>&1; then \
		mkdcdisc -e $(TARGET) -o fox_demo.cdi; \
	elif [ -f /opt/toolchains/dc/bin/mkdcdisc ]; then \
		/opt/toolchains/dc/bin/mkdcdisc -e $(TARGET) -o fox_demo.cdi; \
	elif [ -f $(HOME)/Documents/Dreamcast-Coding/mkdcdisc/builddir/mkdcdisc ]; then \
		$(HOME)/Documents/Dreamcast-Coding/mkdcdisc/builddir/mkdcdisc -e $(TARGET) -o fox_demo.cdi; \
	else \
		echo "Error: mkdcdisc not found. Please set MKDCDISC variable or install mkdcdisc."; \
		exit 1; \
	fi
	@echo "Created: fox_demo.cdi"

# Cleans the binary ELF file plus the intermediate .o files
clean:
	-rm -f $(TARGET) $(OBJS) *.cdi romdisk.img

.PHONY: all clean cdi