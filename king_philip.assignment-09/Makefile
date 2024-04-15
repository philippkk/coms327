poke: poke.o heap.o character.o pokeparser.o
	g++ poke.o heap.o character.o pokeparser.o -o poke -lm -lcurses -std=c++11
poke.o: poke.cpp
	g++ -Wall -Werror -g poke.cpp -c -std=c++11
character.o: helpers/character.cpp
	g++ -Wall -Werror -g helpers/character.cpp -c 
heap.o: helpers/heap.c helpers/heap.h
	gcc -Wall -Werror -g helpers/heap.c -c
pokeparser.o: helpers/pokeparser.cpp
	g++ -Wall -Werror -g helpers/pokeparser.cpp -c -std=c++11

clean:
	rm -f *.o stack heap character poke *~ core
