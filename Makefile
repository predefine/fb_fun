osname := $(shell uname -s)

#use Android toolchain on OS X
#use linaro bare metal toolchain on linux
ifeq ($(osname), Darwin)
CROSS_PREFIX=aarch64-linux-android-
else
CROSS_PREFIX=arm-linux-gnueabi-
endif

CFLAGS :=

all: test.elf test.bin boot.img

OBJECTS := startup.o test.o

# libfdt
LIBFDT_dir := dtc/libfdt/
LIBFDT_OBJS := fdt.o fdt_ro.o fdt_strerror.o
OBJECTS += $(addprefix $(LIBFDT_dir),$(LIBFDT_OBJS)) libfdt_dep.o
CFLAGS += -I$(LIBFDT_dir)

%.o: %.c
	$(CROSS_PREFIX)gcc $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CROSS_PREFIX)as $< -o $@

test.elf: $(OBJECTS)
	 $(CROSS_PREFIX)ld -Ttest.ld $^ -o $@

test.bin: test.elf fix_kernel.py
	$(CROSS_PREFIX)objcopy -O binary $< $@
	python3 fix_kernel.py $@

smalldtb.dtb: smalldtb.dts
	dtc -I dts -O dtb $< -o $@

test.bin-dtb: test.bin smalldtb.dtb
	cat $^ > $@

boot.img: test.bin-dtb
	mkbootimg --kernel test.bin-dtb --base 0x80000000 --ramdisk_offset 0x04000000 --tags_offset 0xe000000 --cmdline "bootopt=64S3,32S1,32S1" -o boot.img

clean:
	rm -f $(OBJECTS)
