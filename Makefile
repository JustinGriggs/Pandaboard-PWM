obj-m = pwm.o
KVERSION = $(shell uname -r)
all:
	make -I /usr/src/linux-headers-$(KVERSION)/arch/arm/plat-omap/include -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
