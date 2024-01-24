#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAPWIDTH 80
#define MAPHEIGHT 21

//% = border, boulder, mountain
// ^ = tree and forest
// # = road
// C and M = marts
// : = long grass
// . = clearing in grass
// ~ = water
// @ = player
// letter = npc

char *map[MAPHEIGHT][MAPWIDTH];

void initMap();
void printMap();
int main(int argc, char *argv[]){
	initMap();
	printMap();
	return 1;
}

void initMap(){
	//to get random rand seed
	srand ( time(NULL) );
	int i,j;
	//random int between 0 and 19 
	//int r = rand() % 20;
	//need 2 tall grass, 1 water, and two short grass
	//exits are random on top and bottom
	bool seeded = false;
	int tallGrass = 0,shortGrass = 0,water =0,tree = 0,rock=0;
	for(i = 0; i < MAPHEIGHT; i++){
		for(j= 0; j < MAPWIDTH; j++){
			if(i == 0 || i == MAPHEIGHT - 1 || j == 0 || j == MAPWIDTH -1){
				map[i][j] = "%%";
			}else{
				map[i][j] = "-";
			}

		}
	}
	while(!seeded){
		int x,y;
		x = rand() % 20;x++;//1-20
		y = rand() % 79;y++;//1-79
		if(map[x][y] != "-"){
			continue;
		}
		if(tallGrass < 3){
			map[x][y] = ":";
			tallGrass++;
			continue;
		}
		if(shortGrass < 3){
			map[x][y] = ".";
			shortGrass++;
			continue;
		}
		if(water < 1){
			map[x][y] = "~";
			water++;
			continue;
		}
		if(rock < 1){
			map[x][y] = "%%";
			rock++;
			continue;
		}
		if(tree < 3){
			map[x][y] = "^";
			tree++;	
			continue;
		}
		if(tallGrass > 2 && shortGrass > 2 && water > 0 && tree > 2&& rock >0){
			seeded= true;
		}
	}

	bool growing = true;
	while(growing){
		int i,j;
		for(i = 1; i < MAPHEIGHT; i++){
			for(j=1; j < MAPWIDTH; j++){
				
			}
			if(i == MAPHEIGHT -1){
				growing = false;
			}
		}
	}
}

void printMap(){
	int i,j;
	for(i = 0; i < MAPHEIGHT; i++){
		for(j= 0; j < MAPWIDTH; j++){
			printf(map[i][j]);
			if(j == MAPWIDTH -1){
				printf("\n");
			}
		}
	}
}
