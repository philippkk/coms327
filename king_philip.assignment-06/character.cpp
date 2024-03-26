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
                    }
character::character(){}