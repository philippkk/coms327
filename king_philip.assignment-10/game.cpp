#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <cstdbool>
#include <cstring>
#include <cmath>
#include <cctype>
#include <ncurses.h>
#include <climits>
#include <unistd.h>
#include <random>
#include <inttypes.h>

#include "characters.h"
#include "map.h"

#define COLOR_DARK_BROWN 10
#define COLOR_DARK_GRASS 12

/*
IDEA:
    top down infinite way shooter
    wasd ijkl movement and shootin
    need map, player, enemy classes
        player needs bullet object to shoot
*/

//classes

//globals
int command;
int lastCommand;
map gameMap;
player *playerObj;

//functions
void printScreen();
void handlePlayerPhysics();

void setUpGame(){
    initscr();			/* Start curses mode 		  */
	noecho();
	raw();
	curs_set(0);
    cbreak();
	keypad(stdscr, TRUE);
	start_color();
    nodelay(stdscr,TRUE);
    command = lastCommand = ' ';
    gameMap = map();
    playerObj = gameMap.playerObj;
    
	init_color(COLOR_DARK_GRASS,51,424,79);
    init_pair(1,COLOR_BLACK,COLOR_DARK_GRASS);    
}
int counter = 0;
int keyUpTimer = 0;
int commandERRtimer = 0;
double speed = 0.5;
int main(int argc, char *argv[]){
    setUpGame();
    
    while (command != 'Q')
    {
        counter++;
        command = getch();
        if(command != ERR && command != lastCommand){
            lastCommand = command;
            keyUpTimer = 0;
        }
        if(command == ERR && command != lastCommand){
            commandERRtimer++;
            if(commandERRtimer > 1){
                keyUpTimer++;
                commandERRtimer =0;
            }
        }
        
        handlePlayerPhysics();
        printScreen();
        usleep(16000);
    }
    


    endwin();//end curses for end of game
    return 0;

}
void printMap(){
    
    for(int i = 0; i < 40;i++){
        for(int j = 0; j < 150; j++){
            attron(COLOR_PAIR(gameMap.tiles[i][j]->colorIndex));
            addch(gameMap.tiles[i][j]->tile_str);
            if(j == 149){addstr("\n");}
            attroff(COLOR_PAIR(gameMap.tiles[i][j]->colorIndex));
        }
    }
    mvaddch(playerObj->posY,playerObj->posX,playerObj->symbol);
}
void printScreen(){
    erase();
    printMap();
    mvaddstr(10,10,"hello world");
    mvaddstr(0,5,"command:");
    mvaddch(0,13,lastCommand);
    char str[10];
	snprintf(str,10,"%d",counter);
    mvaddstr(1,0,"X");
    mvaddstr(1,10,"Y");
    mvaddstr(0,0,str);
    snprintf(str,10,"%d",playerObj->posX);
    mvaddstr(2,0,str);
    snprintf(str,10,"%d",playerObj->posY);
    mvaddstr(2,10,str);
    snprintf(str,10,"%f",playerObj->subX);
    mvaddstr(3,0,str);
    snprintf(str,10,"%f",playerObj->subY);
    mvaddstr(3,10,str);

    refresh();
}

void handlePlayerPhysics(){
    if(keyUpTimer > 5){
            lastCommand = ' ';
            keyUpTimer = 0;
        }
        if(lastCommand == 'w'){
            playerObj->accY -= 1.00 * speed;
        }
        if(lastCommand == 'a'){
            playerObj->accX-= 1.25 * speed;
        }
        if(lastCommand == 's'){
            playerObj->accY+= 1.00 * speed;
        }
        if(lastCommand == 'd'){
            playerObj->accX+= 1.25 * speed;
        }
    
    double xamount =  (playerObj->accX);
   
    double yamount =  (playerObj->accY);
   
    playerObj->subX += xamount;
    playerObj->subY += yamount;
    playerObj->posX = round(playerObj->subX);
    playerObj->posY = round(playerObj->subY);
    if(playerObj->accX > 0){
            if(playerObj->accX < 1.075)
                playerObj->accX = 0;
            else
                playerObj->accX -= 1.075;
        }
        else if(playerObj ->accX < 0){
            if(playerObj->accX > -1.075)
                playerObj->accX = 0;
            else
                playerObj->accX += 1.075;

        }
    if(playerObj->accX > 0.15){
        playerObj->accX = 0.15;
    }
    if(playerObj->accX  < -0.15){
        playerObj->accX = -0.15;
    }
    
    if(playerObj->accY > 0){
        if(playerObj->accY < 1.075)
            playerObj->accY = 0;
        else
            playerObj->accY -= 1.075;
    }
    else if(playerObj ->accY < 0){
        if(playerObj->accY > -1.075)
            playerObj->accY = 0;
        else
            playerObj->accY += 1.075;
    }
    if(playerObj->accY >0.025){
        playerObj->accY = 0.025;
    }
     if(playerObj->accY <-0.025){
        playerObj->accY = -0.025;
    }
}