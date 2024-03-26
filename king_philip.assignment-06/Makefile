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


poke: poke.o heap.o character.o
	g++ poke.o heap.o character.o -o poke -lm -lcurses -std=c++11
poke.o: poke.cpp
	g++ -Wall -Werror -g poke.cpp -c -std=c++11
character.o: character.cpp
	g++ -Wall -Werror -g character.cpp -c 
heap.o: heap.c heap.h
	gcc -Wall -Werror -g heap.c -c

clean:
	rm -f *.o stack heap character poke *~ core

