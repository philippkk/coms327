#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
	return 0;
}

void initMap(){
	//to get random rand seed
	srand ( time(NULL) );
	int i,j;
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
	bool seeded = false;
	//seeding here 
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

	//actually growing those seeds here
	bool growing = true;
	bool foundEmpty = false;
	char *temp[MAPHEIGHT][MAPWIDTH];
	memcpy(&temp,&map,sizeof temp);
	// printf("tempVVVV \n");
	// int p,q;
	// for(p = 0; p < MAPHEIGHT; p++){
	// 	for(q= 0; q < MAPWIDTH; q++){
	// 		printf(temp[p][q]);
	// 		if(q == MAPWIDTH -1){
	// 			printf("\n");
	// 		}
	// 	}
	// }
	while(growing){
		int i,j;
		//FIX FOR TOMORROW, YOU DIDNT REALIZE THAT THE LOOP WIll FIND
		//THE LOWER PART AND JUST EXPAND DOWN
		for(i = 1; i < MAPHEIGHT-1; i++){
			for(j=1; j < MAPWIDTH-1; j++){
				if(map[i][j] == "-"){
					foundEmpty = true;
				}

				if(map[i][j] != "-"){
					//check surroundings
					if(map[i-1][j] == "-"){
						temp[i-1][j] = map[i][j];
					}
					if(map[i+1][j]=="-"){
						temp[i+1][j] = map[i][j];
					}
					if(map[i][j-1]=="-"){
						temp[i][j-1] = map[i][j];
					}
					if(map[i][j+1]== "-"){
						temp[i][j+1] = map[i][j];
					}
					continue;
				}

			}			
		}
	
		memcpy(&map,&temp,sizeof map);
		if(!foundEmpty){growing=false;}
		foundEmpty = false;
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
