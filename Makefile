all:	lang-ptbr/ipvi lang-en/ipvi

lang-ptbr/ipvi:	ipvi-b1.c ipvi-msg.h
	gcc -Wall -O2 -DMSG_PTBR -o lang-ptbr/ipvi ipvi-b1.c

lang-en/ipvi:	ipvi-b1.c ipvi-msg.h
	gcc -Wall -O2 -DMSG_EN -o lang-en/ipvi ipvi-b1.c

	