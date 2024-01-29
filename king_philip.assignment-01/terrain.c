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

char *map[MAPHEIGHT][MAPWIDTH];
int weightmap[MAPHEIGHT][MAPWIDTH];

void initMap();
void printMap();
void genPaths();

int main(int argc, char *argv[]){
	srand ( time(NULL) );
	initMap();
	printMap();
	return 0;
}

void initMap(){
	//to get random rand seed
	int i,j;
	int tallGrass = 0,shortGrass = 0,water =0,tree = 0,rock=0;
	for(i = 0; i < MAPHEIGHT; i++){
		for(j= 0; j < MAPWIDTH; j++){
			if(i == 0 || i == MAPHEIGHT - 1 || j == 0 || j == MAPWIDTH -1){
				map[i][j] = "%%";
				weightmap[i][j] = 9;
			}else{
				map[i][j] = "-";
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
	//copy map to a tempa array to do growing calcs
	memcpy(&temp,&map,sizeof temp);
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
				if(map[i][j] == "-"){
					foundEmpty = true;
				}

				if(map[i][j] != "-"){
					//check surroundings
					if(x > 7){
						if(map[i-1][j] == "-"){
							temp[i-1][j] = map[i][j];
							weightmap[i-1][j] += weight;
						}
						if(map[i+1][j]=="-"){
							temp[i+1][j] = map[i][j];
							weightmap[i+1][j] += weight;
						}
					}else{
						if(map[i][j-1]=="-"){
							temp[i][j-1] = map[i][j];
							weightmap[i][j-1] += weight;
						}
						if(map[i][j+1]== "-"){
							temp[i][j+1] = map[i][j];
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
		//copy temp back to map
		memcpy(&map,&temp,sizeof map);
		if(!foundEmpty){growing=false;}
		foundEmpty = false;
	}
	
	genPaths();
	
}
void genPaths(){
	int a,b,c,d;// coord of each exit;
	a = rand() % 70;a+= 5;
	b = rand() % 70;b+= 5;
	c = rand() % 12;c+= 4;
	d = rand() % 12;d+= 4;

	//height width
	map[0][a] = "a";
	map[MAPHEIGHT-1][b] = "b";
	map[c][0]= "c";
	map[d][MAPWIDTH -1] = "d";


	//take weight map, start at the exits and use the weights to make it to the next point.
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
				}else if(weightmap[y][x+1] < weightmap[y][x]){
					x++;
				}else if(weightmap[y+1][x+1] < weightmap[y][x]){
					x++;
					y++;
				}else{
					y++;
				}
			}else if (x > targetx && x != targetx){//target to left
				if(abs(targetx-x) >= abs(targety-y)){
					if(y == 0){y++;}
					int ran = rand() % 10;
					if(ran > 7 && y != 0 && x < a - 1){y++;}
					x--;
				}else if(weightmap[y][x-1] < weightmap[y][x]){
					x--;
				}else if(weightmap[y+1][x-1] < weightmap[y][x]){
					x--;
					y++;
				}else{
					y++;
				}
			}else{
				y++;
			}
			
			
			map[y][x] = "#";
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
				}else if(weightmap[y][x+1] < weightmap[y][x]){
					x++;
				}else if(weightmap[y+1][x+1] < weightmap[y][x] && targety != y){
					x++;
					y++;
				}else{
					x++;
				}
			}else if (y > targety && y != targety){
				if(abs(targety-y) >= abs(targetx-x)){
					if(x == 0){x++;}
					int ran = rand() % 10;
					if(ran > 7 && x != 0 && y < c + 1){x++;}
					y--;
				}else if(weightmap[y][x+1] < weightmap[y][x]){
					x++;
				}else if(weightmap[y-1][x+1] < weightmap[y][x] && targety != y){
					x++;
					y--;
				}else{
					x++;
				}

			}else{
				x++;
			}
			
			
				map[y][x] = "#";
			
		}else{
			cconnectd = true;
		}
	}
	map[0][a] = "#";
	map[MAPHEIGHT-1][b] = "#";
	map[c][0]= "#";
	map[d][MAPWIDTH -1] = "#";
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
	// for(i = 0; i < MAPHEIGHT; i++){
	// 	for(j= 0; j < MAPWIDTH; j++){

	// 		printf("%1d",weightmap[i][j]);
	// 		if(j == MAPWIDTH -1){
	// 			printf("\n");
	// 		}
	// 	}
	// }
}

