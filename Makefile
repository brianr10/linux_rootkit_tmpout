obj-m += example-rootkit.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules compile_commands.json

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
