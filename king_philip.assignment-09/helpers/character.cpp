#include "character.h"

character::character(int x,int y,char *s,
                    int d,char *t,int nt,int de)
                    {    
                        posX = x;
                        posY = y;
                        symbol = s;
                        dir = d;
                        tile = t;
                        nextTurn = nt;
                        defeated = de;
                        pokemon[0] = pokemonObject();
                        pokemon[1] = pokemonObject();
                        pokemon[2] = pokemonObject();
                        pokemon[3] = pokemonObject();
                        pokemon[4] = pokemonObject();
                        pokemon[5] = pokemonObject();
                    }
character::character(){}