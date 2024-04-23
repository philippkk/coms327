#include "map.h"

map_tile::map_tile(char tile,int colorId){
    tile_str = tile;
    colorIndex = colorId;
};
map::map(){
    for(int i = 0; i < 40;i++){
        for(int j = 0; j < 150; j++){
            tiles[i][j] = new map_tile(' ',1);
            enemies[i][j] = new enemy(' ');
            projectiles[i][j] = new projectile(' ');

        }
    }

    playerObj = new player('@');
    playerObj->posX = playerObj->posY = playerX = playerY = playerObj->subX = playerObj->subY = 20;
};

