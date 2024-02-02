#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAPWIDTH 80
#define MAPHEIGHT 21
#define INFINITY 9999

//% = border, boulder, mountain
// ^ = tree and forest
// # = road
// C and M = marts
// : = long grass
// . = clearing in grass
// ~ = water
// @ = player
// letter = npc

typedef struct map{
	int i;
	char *tiles[MAPHEIGHT][MAPWIDTH];
}map;

map *maps[401][401];

char *oldMap[MAPHEIGHT][MAPWIDTH];
int weightmap[MAPHEIGHT][MAPWIDTH];

void initMap();
void printMap();
void genPaths();
void genBuildings();

int main(int argc, char *argv[]){
	srand ( time(NULL) );
	map *m;
	initMap();
	printMap();
	m->i = 5;
	maps[0][0] = m;
	printf(" %d \n",m->i);
	printf(" %d \n",maps[0][0]->i);
	return 0;
}

void initMap(){
	//to get random rand seed
	int i,j;
	int tallGrass = 0,shortGrass = 0,water =0,tree = 0,rock=0;
	for(i = 0; i < MAPHEIGHT; i++){
		for(j= 0; j < MAPWIDTH; j++){
			if(i == 0 || i == MAPHEIGHT - 1 || j == 0 || j == MAPWIDTH -1){
				oldMap[i][j] = "%%";
				weightmap[i][j] = 9;
			}else{
				oldMap[i][j] = "-";
				weightmap[i][j] = 0;
			}
		}
	}
	bool seeded = false;
	//seeding here 
	while(!seeded){
		int x,y;
		x = rand() % 20;x++;//1-20
		y = rand() % 79;y++;//1-79
		if(oldMap[x][y] != "-"){
			continue;
		}
		if(tallGrass < 3){
			oldMap[x][y] = ":";
			tallGrass++;
			continue;
		}
		if(shortGrass < 3){
			oldMap[x][y] = ".";
			shortGrass++;
			continue;
		}
		if(water < 1){
			oldMap[x][y] = "~";
			water++;
			continue;
		}
		if(rock < 1){
			oldMap[x][y] = "%%";
			rock++;
			continue;
		}
		if(tree < 3){
			oldMap[x][y] = "^";
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
	//copy oldMap to a tempa array to do growing calcs
	memcpy(&temp,&oldMap,sizeof temp);
	while(growing){
		
		int x,y;	
		int i,j;
		int pos = rand() % 10;
		int weight = rand() % 10;
		if(pos > 6){
			weight *= -1;
		}
		for(i = 1; i < MAPHEIGHT-1; i++){
			for(j=1; j < MAPWIDTH-1; j++){
				x = rand() % 10;//0-9
				if(oldMap[i][j] == "-"){
					foundEmpty = true;
				}

				if(oldMap[i][j] != "-"){
					//check surroundings
					if(x > 7){
						if(oldMap[i-1][j] == "-"){
							temp[i-1][j] = oldMap[i][j];
							weightmap[i-1][j] += weight;
						}
						if(oldMap[i+1][j]=="-"){
							temp[i+1][j] = oldMap[i][j];
							weightmap[i+1][j] += weight;
						}
					}else{
						if(oldMap[i][j-1]=="-"){
							temp[i][j-1] = oldMap[i][j];
							weightmap[i][j-1] += weight;
						}
						if(oldMap[i][j+1]== "-"){
							temp[i][j+1] = oldMap[i][j];
							weightmap[i][j+1] += weight;
						}
					}
				}
			if(weightmap[i][j] < 0){
				weightmap[i][j] = 0;
			}
			if(weightmap[i][j] > 8){
				weightmap[i][j] = 8;
			}
			}			
		}
		//copy temp back to oldMap
		memcpy(&oldMap,&temp,sizeof oldMap);
		if(!foundEmpty){growing=false;}
		foundEmpty = false;
	}
	
	genPaths();
	genBuildings();
	
}
void genPaths(){
	int a,b,c,d;// coord of each exit;
	a = rand() % 70;a+= 5;
	b = rand() % 70;b+= 5;
	c = rand() % 12;c+= 4;
	d = rand() % 12;d+= 4;
	//take weight oldMap, start at the exits and use the weights to make it to the next point.
	bool aconnectb = false;
	bool cconnectd = false;
	int x = a;
	int y = 0;
	int targetx = b;
	int targety = MAPHEIGHT - 1;
	while(!aconnectb){
		//set up logic
		if(y <= targety){
			if(x < targetx && x != targetx){
				if(abs(targetx-x) >= abs(targety-y)){
					if(y == 0){y++;}
					int ran = rand() % 10;
					if(ran > 7 && y != 0 && x > a + 1){y++;}
					x++;
					oldMap[y][x-1] = "#";
				}else if(weightmap[y][x+1] < weightmap[y][x]){
					x++;
				}else if(weightmap[y+1][x+1] < weightmap[y][x]){
					y++;
					x++;
					oldMap[y][x-1] = "#";
				}else{
					y++;
				}
			}else if (x > targetx && x != targetx){//target to left
				if(abs(targetx-x) >= abs(targety-y)){
					if(y == 0){y++;}
					int ran = rand() % 10;
					if(ran > 7 && y != 0 && x < a - 1){y++;}
					x--;
					oldMap[y][x+1] = "#";
				}else if(weightmap[y][x-1] < weightmap[y][x]){
					x--;
				}else if(weightmap[y+1][x-1] < weightmap[y][x]){
					y++;
					x--;
					oldMap[y][x+1] = "#";
				}else{
					y++;
				}
			}else{
				y++;
			}
			
			
			oldMap[y][x] = "#";
		}else{
			aconnectb = true;
		}
	}	
	 x = 0;
	 y = c;
	 targetx = MAPWIDTH -2;
	 targety = d;
	while(!cconnectd){
				//set up logic
		if(x <= targetx){
			//target is down
			if(y < targety && y != targety){
				if(abs(targety-y) >= abs(targetx-x)){
					if(x == 0){x++;}
					int ran = rand() % 10;
					if(ran > 7 && x != 0 && y > c + 1){x++;}
					y++;
					oldMap[y-1][x]= "#";
				}else if(weightmap[y][x+1] < weightmap[y][x]){
					x++;
				}else if(weightmap[y+1][x+1] < weightmap[y][x] && targety != y){
					x++;
					y++;
					oldMap[y-1][x]= "#";
					
				}else{
					x++;
				}
			}else if (y > targety && y != targety){
				if(abs(targety-y) >= abs(targetx-x)){
					if(x == 0){x++;}
					int ran = rand() % 10;
					if(ran > 7 && x != 0 && y < c + 1){x++;}
					y--;
					oldMap[y+1][x]= "#";
				}else if(weightmap[y][x+1] < weightmap[y][x]){
					x++;
				}else if(weightmap[y-1][x+1] < weightmap[y][x] && targety != y){
					x++;
					y--;
					oldMap[y+1][x]= "#";
				}else{
					x++;
				}

			}else{
				x++;
			}
			
				oldMap[y][x] = "#";
			
		}else{
			cconnectd = true;
		}
	}
	oldMap[0][a] = "#";
	oldMap[MAPHEIGHT-1][b] = "#";
	oldMap[c][0]= "#";
	oldMap[d][MAPWIDTH -1] = "#";
}
void genBuildings(){
	int x,y;
	bool mart = false,center = false;

	while(!mart || !center){
		int spot = 0;
		x = rand() % 60; x += 10;
		y = rand() % 12; y += 5;
		if(oldMap[y][x] == "#"){
		//check for mart
			int num = rand() % 4;
			switch (num)
			{
			case 0://check left
				if(oldMap[y][x-1] != "#" && oldMap[y][x-2] != "#" &&
				oldMap[y-1][x-1] != "#" && oldMap[y-1][x-2] != "#"){
					spot = 1;
				}
				break;
			case 1://check right
				if(oldMap[y][x+1] != "#" && oldMap[y][x+2] != "#" &&
				oldMap[y-1][x+1] != "#" && oldMap[y-1][x+2] != "#"){
					spot = 2;
				}
				break;
			case 2://check up
				if(oldMap[y+1][x] != "#" && oldMap[y+2][x] != "#" &&
				oldMap[y+1][x+1] != "#" && oldMap[y+2][x+1] != "#"){
					spot = 3;
				}
				break;
			case 3://check down
				if(oldMap[y-1][x] != "#" && oldMap[y-2][x] != "#" &&
				oldMap[y-1][x+1] != "#" && oldMap[y-2][x+1] != "#"){
					spot = 4;
				}
				break;	
			default:
				break;
			}

			if(spot != 0){
				char *c;
				if(!mart){
					c = "M";
					mart = true;
				}else{
					c = "P";
					center = true;
				}
				switch(spot){
					case 1:
					oldMap[y][x-1] = c;oldMap[y][x-2] = c;oldMap[y-1][x-1] = c;oldMap[y-1][x-2] = c;
						break;
					case 2:
					oldMap[y][x+1] = c;oldMap[y][x+2] = c;oldMap[y-1][x+1] = c;oldMap[y-1][x+2] = c;
						break;
					case 3:
					oldMap[y+1][x] = c;oldMap[y+2][x] = c;oldMap[y+1][x+1] = c;oldMap[y+2][x+1] = c;
						break;
					case 4:
					oldMap[y-1][x] = c;oldMap[y-2][x] = c;oldMap[y-1][x+1] = c;oldMap[y-2][x+1] = c;
						break;
				}
			}
		}
	}
}
void printMap(){
	int i,j;
	for(i = 0; i < MAPHEIGHT; i++){
		for(j= 0; j < MAPWIDTH; j++){
			printf(oldMap[i][j]);
			if(j == MAPWIDTH -1){
				printf("\n");
			}
		}
	}
}

