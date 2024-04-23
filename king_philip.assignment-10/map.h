#ifndef MAP_H
#define MAP_H
#include "characters.h"
class map_tile{
    public:
        int colorIndex;
        char tile_str;
        map_tile(char tile,int colorId);
};

class map{
    public:
        player *playerObj;
        map_tile *tiles[40][150];
        enemy *enemies[40][150];
        projectile *projectiles[40][150];
        int playerX,playerY;
        map();
};
#endif