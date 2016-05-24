# Use this either on the BeagleBone Black to compile natively,
# or set CROSS_COMPILE to point at your cross toolchain and
# compile on your host machine

CC=$(CROSS_COMPILE)gcc
CFLAGS=-Wall -g
PROGS=i2c-demo i2c-pwm-demo gpio-int gpio-onoff

all: $(PROGS)

i2c-demo: i2c-demo.c

gpio-int: gpio-int.c

gpio-onoff: gpio-onoff.c

i2c-pwm-demo: i2c-pwm-demo.c

clean:
	rm -f *.o
	rm -f $(PROGS)

install:
	cp $(PROGS) $(TARGET_DIR)/usr/local/bin
