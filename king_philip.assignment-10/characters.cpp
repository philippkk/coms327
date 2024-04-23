#include "characters.h"

player::player(char s){
    symbol = s;
    accX = accY = 0;
}   
enemy::enemy(char s){
    symbol = s;
}
projectile::projectile(char s){
    symbol = s;
}