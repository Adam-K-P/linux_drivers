TARGET = mem_test

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= $(TARGET).o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build 
PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules

endif

client:
	gcc -std=gnu99 -Wall -Wextra -o test mem_test_client.c

clean:
	rm -rf *.o *.ko *~ core .depend *.mod.c .*.cmd .tmp_versions .*.o.d *.swp test

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend

ins: default rem
	insmod ./$(TARGET).ko 

rem:
	@if [ -n "`lsmod | grep -s $(TARGET)`" ]; then rmmod $(TARGET); echo "rmmod $(TARGET)"; fi

test:
	gcc -std=c11 -O3 -o test test.c

ifeq (.depend,$(wildcard .depend))
include .depend
endif
