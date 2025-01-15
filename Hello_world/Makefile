#obj-m += hello_world.o
#obj-m += Major_Minor.o
obj-m += Device_node.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
