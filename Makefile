gadgetrie: gadgetrie.o gadget.o gadget_list.o misc.o
	gcc -ldisasm -lelf gadgetrie.o gadget_list.o gadget.o misc.o -o gadgetrie

gadget.o: gadget.c gadget.h
	gcc -Wall -c gadget.c

gadgetrie.o: gadgetrie.c
	gcc -Wall -c gadgetrie.c

gadget_list.o: gadget_list.c
	gcc -Wall -c gadget_list.c

misc.o: misc.c
	gcc -Wall -c misc.c

tester: tester.c
	gcc -m32 -static tester.c -o tester

clean:
	rm *.o gadgetrie
