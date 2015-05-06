ifneq ($(KERNELRELEASE),)
mymodule-objs :=skm3.c
obj-m += skm3.o
else
PWD := $(shell pwd)
KVER := $(shell uname -r)
KDIR := /lib/modules/$(KVER)/build
all:
	rm -rf *.o *.mod.c *.symvers *order *.markers *.cmd *-
	gcc -o test3  test3.c
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	rm -rf *.o *.mod.c *.symvers *order *.markers *.cmd *-
endif
