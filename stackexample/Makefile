stack: stack.o main.o
	gcc stack.o main.o -o stack
	
stack.o: stack.c stack.h
	gcc -Wall -Werror -g stack.c -c

main.o: main.c stack.h
	gcc -Wall -Werror -g main.c -c

clean:
	rm -f *.o stack
