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
    [TREE]  = "\033[43;37m^\033[0m",
	[ROCK] = "\033[45;37m%%\033[0m",
	[ROAD] = "\033[40;37m#\033[0m",
    [LONG] = "\033[42;37m:\033[0m",
	[SHORT] = "\033[42;37m.\033[0m",
	[WATER] = "\033[46;37m~\033[0m",
	[MART] = "\033[46;37mM\033[0m",
	[CENTER] = "\033[41;37mP\033[0m",
};

enum Commands{
	EMPTY,
	N,
	E,
	S,
	W,
	FLY,
	Q
};
char * const command_str[]={
	[EMPTY] = "",
	[N] = "N",
	[S] = "S",
	[E] = "E",
	[W] = "W",
	[FLY] = "F",
	[Q] = "Q"
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

	    srand ( time(NULL) );
		if(maps[posy][posx] == NULL){
			maps[posy][posx] = createMap();
		}
		currentMap = createMap();
		//printf("command: %s\n",command);
		if(!maps[posy][posx]->generated){
			//printf("first init");
			initMap(99,99,99,99);
		}


		printf("\033[96mEnter command: \033[0m");
		//scanf("%s",command);
		fgets(command,20,stdin);
		char *s = command;
		 while (*s) {
    		*s = toupper((unsigned char) *s);
   		 	s++;
  		}
		s[strlen(s)-1] = '\0'; //get rid of newline from the fgets()
		char copyStr[20];
		strcpy(copyStr,command);
		if(strcmp(command,command_str[EMPTY])){
			if(!strcmp(command,command_str[Q])){
				break;
			}else if(!strcmp(command,command_str[N])){
				posy--;	
				loadMap();
			}else if(!strcmp(command,command_str[S])){
				posy++;
				loadMap();
			}else if(!strcmp(command,command_str[E])){
				posx++;
				loadMap();
			}else if(!strcmp(command,command_str[W])){
				posx--;
				loadMap();
			}else if(!strcmp(strtok(copyStr, " "),command_str[FLY])){
				int i,x,y;
				for (i = 0; command[i] != '\0'; ++i)
					;
				if(i > 11){
					printf("INVALID FLY!!!!\n");
					continue;
				}
				char * pch;
				int counter = 0;
				pch = strtok (command," ,.");
				while (pch != NULL)
				{
					int i;
					for (i = 0; pch[i] != '\0'; ++i);
					if(i > 4 || atoi(pch) < -200 || atoi(pch) > 200){
						printf("INVALID FLY!!!!\n");
						break ;
					}
					if(counter == 1){
						printf("X: %s\n", pch);
						x = atoi(pch);
					}
					if(counter == 2){
						printf("Y: %s\n", pch);
						y = atoi(pch);
					}

					pch = strtok (NULL, " ,.");
					counter++;
				}
				//should be good to fly now
				posy = y + 200;
				posx = x + 200;
				loadMap();
			}else{
				printf("unknown command! \"%s\" \n",command);
				// printf("%s",&command[0]);
			}

		}
	}
	

	return 0;
}
void loadMap(){
		//printf("(%d,%d)pre\n",posx-200,posy-200);
		//printf("(%d,%d)pre real:\n",posx,posy);
	if(maps[posy][posx] == NULL){
		printf("in loadmap");
		//LOGIC FOR CONNECTING GATES BETWEEN MAPS VVVVVVV
		int a=99,b=99,c=99,d=99;

		if(posy > 0){
			if(maps[posy-1][posx] != NULL){
				if (maps[posy-1][posx]->gateBx != 99){
					a= maps[posy-1][posx]->gateBx;
				}
			}
		}
		if(posy < 400){
			if(maps[posy+1][posx] != NULL){
				if (maps[posy+1][posx]->gateAx != 99){
					b = maps[posy+1][posx]->gateAx;
				}		
			}
		}
		if(posx < 400){
			if(maps[posy][posx+1] != NULL){
				if (maps[posy][posx+1]->gateCy != 99){
					d = maps[posy][posx+1]->gateCy;
				}
			}
		}
		if(posx > 0){
			if(maps[posy][posx-1]!=NULL){
				if (maps[posy][posx-1]->gateDy != 99){
					c = maps[posy][posx-1]->gateDy;
				}
			}
		}

		maps[posy][posx] = createMap();	
		
		initMap(a,b,c,d);
	}else{
		//printf("\nFOUND MAP\n");
		printMap(maps[posy][posx]);
	}
}
void initMap(int a,int b,int c, int d){
	printf("INIT\n");
	//to get random rand seed

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
			//printf("(%d,%d)",i,j);
			// if(j > 70){
			// 	printf("j: %d",j);
			// }
		}
	}
	//printf("after loop");
	bool seeded = false;
	//seeding here 
	while(!seeded){
		int x,y;
		//printf("seed");
		x = rand() % 20;x++;//1-20
		y = rand() % 79;y++;//1-79
		if(*oldMap[x][y] != '-'){
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
		int x;	
		int i,j;
		int pos = rand() % 10;
		int weight = rand() % 10;
		if(pos > 6){
			weight *= -1;
		}
		for(i = 1; i < MAPHEIGHT-1; i++){
			for(j=1; j < MAPWIDTH-1; j++){
				x = rand() % 10;//0-9
				if(*oldMap[i][j] == '-'){
					foundEmpty = true;
				}

				if(*oldMap[i][j] != '-'){
					//check surroundings
					if(x > 7){
						if(*oldMap[i-1][j] == '-'){
							temp[i-1][j] = oldMap[i][j];
							weightmap[i-1][j] += weight;
						}
						if(*oldMap[i+1][j]=='-'){
							temp[i+1][j] = oldMap[i][j];
							weightmap[i+1][j] += weight;
						}
					}else{
						if(*oldMap[i][j-1]=='-'){
							temp[i][j-1] = oldMap[i][j];
							weightmap[i][j-1] += weight;
						}
						if(*oldMap[i][j+1]== '-'){
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
	
	printf("before path a%d b%d c%d d%d \n",a,b,c,d);
	genPaths(a,b,c,d);
	printf("\nafter path\n");
	genBuildings();
	
	for (i = 0;i < MAPHEIGHT;i++){
		for(j=0;j <MAPWIDTH;j++){
			currentMap->tiles[i][j] = oldMap[i][j];
		}
	}  
	copyMap(maps[posy][posx],currentMap);
	printMap(currentMap);
}
void findPath(int x ,int y, int targetx, int targety){
	int counter = 0;
	bool atod = false;
	bool ctob = false;
	bool ctoa = false;
	bool btod = false;
	while(x != targetx || y != targety){
		//c to b left to down
		//c to a left to up
		//a to d up to right
		//b to d down to rigght
		if(x == 0){//c
			if (targety == 1){
				ctoa = true;
			}else if(targety == MAPHEIGHT -2){
				ctob = true;
			}
			x++;
			oldMap[y][x] = tile_str[ROAD];
			continue;
		}
		if(y == 0){//a
			atod = true;
			y++;
			oldMap[y][x] = tile_str[ROAD];
			continue;;
		}
		if(y == MAPHEIGHT -1){//b
			//only b to d
			btod = true;
			y--;
			oldMap[y][x] = tile_str[ROAD];
			continue;;
		}	

		if(atod){
			if(targety > y){
				if(weightmap[y+1][x] < weightmap[y][x]+3){
					y++;;
				}else if(x == targetx){
					y++;
				}
				oldMap[y][x] = tile_str[ROAD];
			}
			if(targetx > x){
				x++;
				oldMap[y][x] = tile_str[ROAD];
			}
		}
		if(btod){
				if(targety < y){
				if(weightmap[y-1][x] < weightmap[y][x] + 3){
					y--;;
				}else if(x == targetx){
					y--;
				}
				oldMap[y][x] = tile_str[ROAD];
			}
			if(targetx > x){
				x++;
				oldMap[y][x] = tile_str[ROAD];
			}
		}
		if(ctoa){
			//need to focus x++ and y--
			if(targety < y){
				if(weightmap[y-1][x] < weightmap[y][x]){
					y--;
				}else if(x == targetx){
					y--;
				}
				oldMap[y][x] = tile_str[ROAD];
			}
			if(targetx > x){
				x++;
				oldMap[y][x] = tile_str[ROAD];
			}
		}
		if(ctob){
			//need to focus x++ and y++
			if(targety > y){
				if(weightmap[y+1][x] < weightmap[y][x]){
					y++;
				}else if(x == targetx){
					y++;
				}
				oldMap[y][x] = tile_str[ROAD];
			}
			if(targetx > x){
				x++;
				oldMap[y][x] = tile_str[ROAD];
			}
		}
		counter ++;
		if(counter == 5000){
			x = targetx;
			y = targety;
			oldMap[8][35] = tile_str[ROAD];
			oldMap[12][45] = tile_str[ROAD];
			printf("force out");
		}
	}
}
void genPaths(int new_a, int new_b, int new_c, int new_d){
	int a,b,c,d;// coord of each exit;
	//printf("in path");
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
	//edge check
	if(posx == 400){
		//bottom right
		if(posy == 400){
			//dont gen b or d
			//bool cconnecta = false;
			x = 0;y = c;
			targetx = a; targety = 1;
			findPath(x,y,targetx,targety);
			oldMap[0][a] = tile_str[ROAD];
			oldMap[c][0]= tile_str[ROAD];
			currentMap->gateAx=a;
			currentMap->gateBx=99;
			currentMap->gateCy=c;
			currentMap->gateDy=99;
		}
		//top right
		else if(posy == 0){
			//dont get a or d
			//bool cconnectb = false;
			x = 0;y=c;
			targetx =b; targety = MAPHEIGHT -2;
			findPath(x,y,targetx,targety);
			oldMap[MAPHEIGHT-1][b] = tile_str[ROAD];
			oldMap[c][0]= tile_str[ROAD];
			currentMap->gateAx=99;
			currentMap->gateBx=b;
			currentMap->gateCy=c;
			currentMap->gateDy=99;
		}else{
			x = 0;y=c;
			targetx =b; targety = MAPHEIGHT -2;
			findPath(x,y,targetx,targety);
			targetx =a; targety = 1;
			findPath(x,y,targetx,targety);		
			oldMap[0][a] = tile_str[ROAD];
			oldMap[MAPHEIGHT-1][b] = tile_str[ROAD];
			oldMap[c][0]= tile_str[ROAD];
			currentMap->gateAx=a;
			currentMap->gateBx=b;
			currentMap->gateCy=c;
			currentMap->gateDy=99;
		}
		//just right dont gen d
		//aconnectb = false;
		//bool cconnecta = false;
	}else if(posx == 0){
		//printf("\nx = 0\n");
		//bottom left			printf("WOOOW\n");
		if(posy == 400){
			//dont gen b or c
			//	bool aconnectd = false;
			x = a; y = 0;
			targetx = MAPWIDTH -2; targety = d;
			findPath(x,y,targetx,targety);
			oldMap[0][a] = tile_str[ROAD];
			oldMap[d][MAPWIDTH -1] = tile_str[ROAD];
			currentMap->gateAx=a;
			currentMap->gateBx=99;
			currentMap->gateCy=99;
			currentMap->gateDy=d;
		}
		//top left
		else if(posy == 0){
			//dont gen a or c
	//		bool bconnectd = false;
			x = b; y = MAPHEIGHT -1;	
			targetx =MAPWIDTH -2; targety = d;
			findPath(x,y,targetx,targety);
			oldMap[MAPHEIGHT-1][b] = tile_str[ROAD];
			oldMap[d][MAPWIDTH -1] = tile_str[ROAD];
			currentMap->gateAx=99;
			currentMap->gateBx=b;
			currentMap->gateCy=99;
			currentMap->gateDy=d;
		}else{
			//just left dont gen c
			//aconnectb = false;
			//bool aconnectd = false;			printf("WOOOW\n");
			x = a; y = 0;
			targetx =MAPWIDTH -2; targety = d;
			findPath(x,y,targetx,targety);
			//now b to d
			x = b; y = MAPHEIGHT -1;
			findPath(x,y,targetx,targety);
			oldMap[0][a] = tile_str[ROAD];
			oldMap[MAPHEIGHT-1][b] = tile_str[ROAD];
			oldMap[d][MAPWIDTH -1] = tile_str[ROAD];
			currentMap->gateAx=a;
			currentMap->gateBx=b;
			currentMap->gateCy=99;
			currentMap->gateDy=d;
		}


	}else if(posy == 0){
		//top dont gen a
	}else if(posy == 400){
		//bottom dont gen b
	}else{
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



}


void genBuildings(){
	int x,y;
	int martx = 1000;
	int marty = 1000;
	bool mart = false,center = false;

	while(!mart || !center){
		printf("building loop %d %d\n",mart,center);
		int spot = 0;
		x = rand() % 76; x += 1;
		y = rand() % 17; y += 2;
		if(oldMap[y][x] == tile_str[ROAD] && x != martx +1 && y != marty +1&& x != martx -1 && y != marty -1 && x != martx && y != marty){
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
					martx = x;
					marty = y;
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
	printf("\033[0mCurrent Map Pos = ");
	printf("(%d,%d)\n",posx-200,posy-200);
//	printf("(%d,%d)\n",posx,posy);
}


map* createMap() {
    map* newMap = malloc(sizeof(map));
    newMap->generated = false;
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

