# Anim4DC Fox Demo Makefile
# Darc was right: "copy/paste example makefile and change TARGET/OBJS and add -lraylib"

TARGET = fox_demo.elf
OBJS = main.o romdisk.o

all: $(TARGET)

# Just add raylib to what we know works
$(TARGET): $(OBJS)
	sh-elf-gcc -O2 -ml -m4-single -ffunction-sections -fdata-sections \
		-I$(KOS_BASE)/include -I$(KOS_BASE)/kernel/arch/dreamcast/include \
		-I$(KOS_BASE)/addons/include -I$(KOS_BASE)/../kos-ports/include \
		-I$(KOS_BASE)/../kos-ports/raylib4dc/inst/include \
		-I$(KOS_BASE)/../kos-ports/libGL/inst/include -I../../include \
		-ml -m4-single -nostartfiles -Wl,--gc-sections \
		-T$(KOS_BASE)/utils/ldscripts/shlelf.xc \
		-L$(KOS_BASE)/lib/dreamcast -L$(KOS_BASE)/addons/lib/dreamcast \
		-L$(KOS_BASE)/../kos-ports/lib \
		-o $@ $(OBJS) \
		-lraylib -lGL -lkallisti -lm -lc -lgcc

%.o: %.c
	sh-elf-gcc -O2 -ml -m4-single -ffunction-sections -fdata-sections \
		-I$(KOS_BASE)/include -I$(KOS_BASE)/kernel/arch/dreamcast/include \
		-I$(KOS_BASE)/addons/include -I$(KOS_BASE)/../kos-ports/include \
		-I$(KOS_BASE)/../kos-ports/raylib4dc/inst/include \
		-I$(KOS_BASE)/../kos-ports/libGL/inst/include -I../../include \
		-c $< -o $@

romdisk.o:
	genromfs -f romdisk.img -d romdisk -V ROMDISK
	sh-elf-objcopy -I binary -O elf32-shl -B sh \
		--redefine-sym _binary_romdisk_img_start=__romdisk_img \
		--redefine-sym _binary_romdisk_img_end=__romdisk_img_end \
		romdisk.img romdisk.o

cdi: $(TARGET)
	@echo "Creating Anim4DC Fox Demo CDI"
	/home/dylan/Documents/Dreamcast-Coding/mkdcdisc/builddir/mkdcdisc \
		-e $(TARGET) \
		-o fox_demo.cdi
	@echo "Created: fox_demo.cdi"

clean:
	-rm -f $(TARGET) $(OBJS) *.cdi romdisk.img

.PHONY: all clean cdi