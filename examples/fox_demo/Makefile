# Anim4DC Fox Demo Makefile
# Showcases vertex animation system with Fox model

KOS_BASE = /opt/toolchains/dc/kos
TARGET = fox_demo.elf
OBJS = main.o romdisk.o

# KOS compiler flags optimized for Dreamcast
KOS_CFLAGS = -O2 -fno-PIC -fno-PIE -fomit-frame-pointer -m4-single -ml -mfsrra -mfsca \
 -ffunction-sections -fdata-sections -matomic-model=soft-imask -ftls-model=local-exec -D__DREAMCAST__ \
 -I$(KOS_BASE)/include -I$(KOS_BASE)/kernel/arch/dreamcast/include -I$(KOS_BASE)/addons/include \
 -I$(KOS_BASE)/../kos-ports/include -I$(KOS_BASE)/../kos-ports/raylib4dc/inst/include \
 -I$(KOS_BASE)/../kos-ports/libGL/inst/include -I../../include \
 -D_arch_dreamcast -D_arch_sub_pristine -Wall -g

# KOS linker flags
KOS_LDFLAGS = -m4-single -ml -Wl,--gc-sections -T$(KOS_BASE)/utils/ldscripts/shlelf.xc -nostartfiles \
 -L$(KOS_BASE)/lib/dreamcast -L$(KOS_BASE)/addons/lib/dreamcast -L$(KOS_BASE)/../kos-ports/lib

# Libraries (raylib + GL + KOS)
LIBS = -Wl,--start-group -lraylib -lGL -lkallisti -lm -lc -lgcc -Wl,--end-group

all: $(TARGET)

%.o: %.c
	sh-elf-gcc $(KOS_CFLAGS) -c $< -o $@

romdisk.o:
	genromfs -f romdisk.img -d romdisk -V ROMDISK
	sh-elf-objcopy -I binary -O elf32-shl -B sh \
		--redefine-sym _binary_romdisk_img_start=__romdisk_img \
		--redefine-sym _binary_romdisk_img_end=__romdisk_img_end \
		romdisk.img romdisk.o

$(TARGET): $(OBJS)
	sh-elf-gcc $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $@ romdisk.o main.o $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET) *.cdi romdisk.img romdisk.o

cdi: $(TARGET)
	@echo "Creating Anim4DC Fox Demo CDI"
	/home/dylan/Documents/Dreamcast-Coding/mkdcdisc/builddir/mkdcdisc \
		-e $(TARGET) \
		-o fox_demo.cdi
	@echo "Created: fox_demo.cdi"

.PHONY: all clean cdi