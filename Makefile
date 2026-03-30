default : servbeuip clibeuip

clibeuip : clibeuip.c
	cc -Wall -Werror -DTRACE -o clibeuip clibeuip.c

servbeuip : servbeuip.c
	cc -Wall -Werror -DTRACE -o servbeuip servbeuip.c

clean :
	rm -f clibeuip servbeuip
