MODULE_NAME := firewall

obj-m := $(MODULE_NAME).o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

all: mod app
mod:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)
app:
	g++ client.cpp -o client -std=c++11
clean:
	rm -fr *.ko *.o *.cmd cmd $(MODULE_NAME).mod.c modules.order Module.symvers

