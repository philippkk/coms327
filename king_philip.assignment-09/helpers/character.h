#ifndef CHARACTER_H
#define CHARACTER_H
#include "heap.h"
#include "pokeparser.h"
typedef class character{
	public:
		heap_node_t *hn;
		int posX;
		int posY;
		int nextX;
		int nextY;
		char *symbol;
		int dir; //0 = left 1 = right 2 = up 3 = down
		char *tile;
		int nextTurn;
		int defeated;
		pokemonObject *currentPoke;
		pokemonObject pokemon[6];
        character(int x,int y,char *s,
                    int d,char *t,int nt,int de);
        character();
}character_c;

#endif