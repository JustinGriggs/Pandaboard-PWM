obj-m = pwm.o
KVERSION = 3.1.0-1282-omap4
INCLUDEDIR = /usr/src/linux=headers-$(VERSION)/arch/arm/plat-omap/include

all:
	make -I $(INCLUDEDIR) -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
