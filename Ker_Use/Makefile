ifneq ($(KERNELRELEASE),)
mymodule-objs :=modules.c
obj-m += modules.o
else
PWD := $(shell pwd)
KVER := $(shell uname -r)
KDIR := /lib/modules/$(KVER)/build
all:
	gcc -o main main.c
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	rm -rf *.o *.mod.c *.symvers *order *.markers *-
endif