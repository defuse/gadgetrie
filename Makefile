gadget_finder: gadget_finder.o gadget.o gadget_list.o
	gcc -ldisasm -lelf gadget_finder.o gadget_list.o gadget.o -o gadget_finder

gadget.o: gadget.c gadget.h
	gcc -Wall -c gadget.c

gadget_finder.o: gadget_finder.c
	gcc -Wall -c gadget_finder.c

gadget_list.o: gadget_list.c
	gcc -Wall -c gadget_list.c

tester: tester.c
	gcc -m32 -static tester.c -o tester
