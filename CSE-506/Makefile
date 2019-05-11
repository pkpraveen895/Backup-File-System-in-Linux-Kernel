#obj-m += sys_cpenc.o

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all: bkpctl
#cpenc

bkpctl: bkpctl.c
	gcc -Wall -Werror -I$(INC)/generated/uapi -I$(INC)/uapi bkpctl.c -o bkpctl -lcrypto -lssl

#cpenc:
#	make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f bkpctl

