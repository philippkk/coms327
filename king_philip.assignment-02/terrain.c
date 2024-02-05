#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
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
enum Tiles                                                                   
{
    TREE,  ROCK,  ROAD, LONG,SHORT,WATER,MART,CENTER, TILE_MAX                                                                                                                
};   

char * const tile_str[] =
{
    [TREE]  = "\033[33m^\033[0m",
	[ROCK] = "\033[35m%%\033[0m",
	[ROAD] = "\033[37m#\033[0m",
    [LONG] = "\033[32m:\033[0m",
	[SHORT] = "\033[32m.\033[0m",
	[WATER] = "\033[36m~\033[0m",
	[MART] = "\033[36mM\033[0m",
	[CENTER] = "\033[31mP\033[0m",
};

enum Commands{
	EMPTY,
	N,
	E,
	S,
	W,
	FLY
};
char * const command_str[]={
	[EMPTY] = "",
	[N] = "N",
	[S] = "S",
	[E] = "E",
	[W] = "W",
	[FLY] = "F"
};

typedef struct map{
	bool generated;
	int gateAx;//top
	int gateBx;//bottom
	int gateCy;//left
	int gateDy;//right
	char *tiles[MAPHEIGHT][MAPWIDTH];
}map;

map *maps[401][401];
int posx=200,posy=200;
map *currentMap;
char *oldMap[MAPHEIGHT][MAPWIDTH];
int weightmap[MAPHEIGHT][MAPWIDTH];

void initMap(int a, int b, int c, int d);
void printMap();
void genPaths(int a, int b, int c, int d);
void genBuildings();
void loadMap();
map* createMap();
void copyMap(map* destination, const map* source);
void freeMap(map* mapToFree) ;

int main(int argc, char *argv[]){
	char command[20];
	//currentMap = *maps[posx][posy];
	while (*command != 'Q')
	{
		if(maps[posy][posx] == NULL){
			maps[posy][posx] = createMap();
		}
		currentMap = createMap();
		printf("command: %s\n",command);
		if(!maps[posy][posx]->generated){
			printf("first init");
			initMap(99,99,99,99);
		}


		printf("\033[31;44m Enter command: \033[0m");
		scanf("%s",&command);
		char *s = command;
		 while (*s) {
    		*s = toupper((unsigned char) *s);
   		 	s++;
  		}

		if(strcmp(command,command_str[EMPTY])){
			if(!strcmp(command,command_str[N])){
				posy++;	
				loadMap();
			}else if(!strcmp(command,command_str[S])){
				posy--;
				loadMap();
			}else if(!strcmp(command,command_str[E])){
				posx++;
				loadMap();
			}else if(!strcmp(command,command_str[W])){
				posx--;
				loadMap();
			}

		}
	}
	

	return 0;
}
void loadMap(){\
	if(maps[posy][posx] == NULL){
		//LOGIC FOR CONNECTING GATES BETWEEN MAPS VVVVVVV
		maps[posy][posx] = createMap();	
		int a=99,b=99,c=99,d=99;


		initMap(a,b,c,d);
	}else{
		printf("\nFOUND MAP\n");
		printMap(maps[posy][posx]);
	}
}
void initMap(int a,int b,int c, int d){
	printf("INIT\n");
	//to get random rand seed
	srand ( time(NULL) );
	int i,j;
	int tallGrass = 0,shortGrass = 0,water =0,tree = 0,rock=0;
	for(i = 0; i < MAPHEIGHT; i++){
		for(j= 0; j < MAPWIDTH; j++){
			if(i == 0 || i == MAPHEIGHT - 1 || j == 0 || j == MAPWIDTH -1){
				oldMap[i][j] = tile_str[ROCK];
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
			oldMap[x][y] = tile_str[LONG];
			tallGrass++;
			continue;
		}
		if(shortGrass < 3){
			oldMap[x][y] = tile_str[SHORT];
			shortGrass++;
			continue;
		}
		if(water < 1){
			oldMap[x][y] = tile_str[WATER];
			water++;
			continue;
		}
		if(rock < 1){
			oldMap[x][y] = tile_str[ROCK];
			rock++;
			continue;
		}
		if(tree < 3){
			oldMap[x][y] = tile_str[TREE];
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
	
	genPaths(a,b,c,d);
	genBuildings();
	
	for (i = 0;i < MAPHEIGHT;i++){
		for(j=0;j <MAPWIDTH;j++){
			currentMap->tiles[i][j] = oldMap[i][j];
		}
	}  
	copyMap(maps[posy][posx],currentMap);
	printMap(currentMap);
}
void genPaths(int new_a, int new_b, int new_c, int new_d){
	int a,b,c,d;// coord of each exit;
	if(new_a == 99){
		a = rand() % 70;a+= 5;
	}else{a = new_a;}
	if(new_b == 99){
		b = rand() % 70;b+= 5;
	}else{b = new_b;}
	if(new_c == 99){
		c = rand() % 12;c+= 4;
	}else{c=new_c;}
	if(new_d == 99){
		d = rand() % 12;d+= 4;
	}else{d=new_d;}
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
					oldMap[y][x-1] = tile_str[ROAD];
				}else if(weightmap[y][x+1] < weightmap[y][x]){
					x++;
				}else if(weightmap[y+1][x+1] < weightmap[y][x]){
					y++;
					x++;
					oldMap[y][x-1] = tile_str[ROAD];
				}else{
					y++;
				}
			}else if (x > targetx && x != targetx){//target to left
				if(abs(targetx-x) >= abs(targety-y)){
					if(y == 0){y++;}
					int ran = rand() % 10;
					if(ran > 7 && y != 0 && x < a - 1){y++;}
					x--;
					oldMap[y][x+1] = tile_str[ROAD];
				}else if(weightmap[y][x-1] < weightmap[y][x]){
					x--;
				}else if(weightmap[y+1][x-1] < weightmap[y][x]){
					y++;
					x--;
					oldMap[y][x+1] = tile_str[ROAD];
				}else{
					y++;
				}
			}else{
				y++;
			}
			
			
			oldMap[y][x] = tile_str[ROAD];
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
					oldMap[y-1][x]= tile_str[ROAD];
				}else if(weightmap[y][x+1] < weightmap[y][x]){
					x++;
				}else if(weightmap[y+1][x+1] < weightmap[y][x] && targety != y){
					x++;
					y++;
					oldMap[y-1][x]= tile_str[ROAD];
					
				}else{
					x++;
				}
			}else if (y > targety && y != targety){
				if(abs(targety-y) >= abs(targetx-x)){
					if(x == 0){x++;}
					int ran = rand() % 10;
					if(ran > 7 && x != 0 && y < c + 1){x++;}
					y--;
					oldMap[y+1][x]= tile_str[ROAD];
				}else if(weightmap[y][x+1] < weightmap[y][x]){
					x++;
				}else if(weightmap[y-1][x+1] < weightmap[y][x] && targety != y){
					x++;
					y--;
					oldMap[y+1][x]= tile_str[ROAD];
				}else{
					x++;
				}

			}else{
				x++;
			}
			
				oldMap[y][x] = tile_str[ROAD];
			
		}else{
			cconnectd = true;
		}
	}
	oldMap[0][a] = tile_str[ROAD];
	oldMap[MAPHEIGHT-1][b] = tile_str[ROAD];
	oldMap[c][0]= tile_str[ROAD];
	oldMap[d][MAPWIDTH -1] = tile_str[ROAD];
	currentMap->gateAx=a;
	currentMap->gateBx=b;
	currentMap->gateCy=c;
	currentMap->gateDy=d;
}
void genBuildings(){
	int x,y;
	bool mart = false,center = false;

	while(!mart || !center){
		int spot = 0;
		x = rand() % 60; x += 10;
		y = rand() % 12; y += 5;
		if(oldMap[y][x] == tile_str[ROAD]){
		//check for mart
			int num = rand() % 4;
			switch (num)
			{
			case 0://check left
				if(oldMap[y][x-1] != tile_str[ROAD] && oldMap[y][x-2] != tile_str[ROAD] &&
				oldMap[y-1][x-1] != tile_str[ROAD] && oldMap[y-1][x-2] != tile_str[ROAD]){
					spot = 1;
				}
				break;
			case 1://check right
				if(oldMap[y][x+1] != tile_str[ROAD] && oldMap[y][x+2] != tile_str[ROAD] &&
				oldMap[y-1][x+1] != tile_str[ROAD] && oldMap[y-1][x+2] != tile_str[ROAD]){
					spot = 2;
				}
				break;
			case 2://check up
				if(oldMap[y+1][x] != tile_str[ROAD] && oldMap[y+2][x] != tile_str[ROAD] &&
				oldMap[y+1][x+1] != tile_str[ROAD] && oldMap[y+2][x+1] != tile_str[ROAD]){
					spot = 3;
				}
				break;
			case 3://check down
				if(oldMap[y-1][x] != tile_str[ROAD] && oldMap[y-2][x] != tile_str[ROAD] &&
				oldMap[y-1][x+1] != tile_str[ROAD] && oldMap[y-2][x+1] != tile_str[ROAD]){
					spot = 4;
				}
				break;	
			default:
				break;
			}

			if(spot != 0){
				char *c;
				if(!mart){
					c = tile_str[MART];
					mart = true;
				}else{
					c = tile_str[CENTER];
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
void printMap(map *Map){
	int i,j;
	for(i = 0; i < MAPHEIGHT; i++){
		for(j= 0; j < MAPWIDTH; j++){
			printf(Map->tiles[i][j]);
			if(j == MAPWIDTH -1){
				printf("\n");
			}
		}
	}
	printf("\033[0mCurrent Map Pos= ");
	printf("%d %d\n",posx,posy);
}


map* createMap() {
    map* newMap = malloc(sizeof(map));
    newMap->generated = false;
    
    // Initialize tiles to NULL or specific values as needed
    for (int i = 0; i < MAPHEIGHT; ++i) {
        for (int j = 0; j < MAPWIDTH; ++j) {
            newMap->tiles[i][j] = NULL;
        }
    }

    return newMap;
}

void copyMap(map* destination, const map* source) {
    destination->generated = true;
	destination->gateAx=source->gateAx;
	destination->gateBx=source->gateBx;
	destination->gateCy=source->gateCy;
	destination->gateDy=source->gateDy;
    // Copy tiles array
    for (int i = 0; i < MAPHEIGHT; ++i) {
        for (int j = 0; j < MAPWIDTH; ++j) {
            destination->tiles[i][j] = strdup(source->tiles[i][j]);
        }
    }

}

