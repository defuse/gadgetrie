gadget_finder: gadget_finder.o gadget.o
	gcc -ldisasm -lelf gadget_finder.o gadget.o -o gadget_finder

gadget.o: gadget.c gadget.h
	gcc -Wall -c gadget.c

gadget_finder.o: gadget_finder.c
	gcc -Wall -c gadget_finder.c

tester: tester.c
	gcc -m32 -static tester.c -o tester
