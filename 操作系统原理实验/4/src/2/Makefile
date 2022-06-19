ifneq ($(KERNELRELEASE),)
obj-m := xzcDrive.o
 
else
KVER := $(shell uname -r)
KDIR := /lib/modules/$(KVER)/build
PWD  := $(shell pwd)
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -o test2 test2.c
clean:
	rm -rf .*.cmd *.o *.mod.c *.ko .tmp_versions modules.*  Module.*
	rm -rf test2
endif