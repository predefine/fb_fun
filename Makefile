osname := $(shell uname -s)

#use Android toolchain on OS X
#use linaro bare metal toolchain on linux
ifeq ($(osname), Darwin)
CROSS_PREFIX=aarch64-linux-android-
else
CROSS_PREFIX=arm-linux-gnueabihf-
endif

all: test.elf test.bin boot.img

test.o: test.c
	$(CROSS_PREFIX)gcc -c $< -o $@

startup.o: startup.s
	$(CROSS_PREFIX)as $< -o $@

test.elf: test.o startup.o
	$(CROSS_PREFIX)ld -Ttest.ld $^ -o $@

test.bin: test.elf
	$(CROSS_PREFIX)objcopy -O binary $< $@
boot.img: test.bin
	mkbootimg --kernel test.bin --base 0x20000000 --kernel_offset 0x00008000 -o boot.img

clean:
	rm -f test.bin test.elf startup.o test.o boot.img
