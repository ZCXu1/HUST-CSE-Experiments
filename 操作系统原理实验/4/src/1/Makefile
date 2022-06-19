obj-m:=xzc.o
KDIR:=/usr/src/linux-headers-5.10.60-amd64-desktop
PWD:=$(shell pwd)

default:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
