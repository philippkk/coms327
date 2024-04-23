game: game.o map.o characters.o 
	g++ game.o map.o characters.o -o game -lm -lcurses -std=c++11
game.o: game.cpp
	g++ -Wall -Werror -g game.cpp -c -std=c++11
map.o: map.cpp
	g++ -Wall -Werror -g map.cpp -c -std=c++11
characters.o: characters.cpp
	g++ -Wall -Werror -g characters.cpp -c -std=c++11
clean:
	rm -f *.o game map character *~ core