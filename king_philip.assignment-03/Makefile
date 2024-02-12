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


terrain: terrain.o heap.o
	gcc terrain.o heap.o -o terrain 

terrain.o: terrain.c
	gcc -Wall -Werror -g terrain.c -c 

heap.o: heap.c heap.h
	gcc -Wall -Werror -g heap.c -c

clean:
	rm -f *.o stack heap terrain *~ core

