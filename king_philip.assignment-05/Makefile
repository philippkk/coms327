#terrain: terrain.c
#	gcc terrain.c -o terrain
#clean:
#	rm -f terrain *~ core
#stack: stack.o main.o
#	gcc stack.o main.o -o stack
	
#stack.o: stack.c stack.h
#	gcc -Wall -Werror -g stack.c -c

#main.o: main.c stack.h
#	gcc -Wall -Werror -g main.c -c


poke: poke.o heap.o
	gcc poke.o heap.o -o poke -lm -lcurses

poke.o: poke.c
	gcc -Wall -Werror -g poke.c -c 

heap.o: heap.c heap.h
	gcc -Wall -Werror -g heap.c -c

clean:
	rm -f *.o stack heap poke *~ core

