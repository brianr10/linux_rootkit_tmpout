obj-m += paradox.o

paradox-objs := paradox.o \
	modules/become_root.o \
	modules/lkrg_bypass.o \
	modules/resist.o \
	modules/core.c \
	ftrace/ftrace_helper.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules compile_commands.json

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
