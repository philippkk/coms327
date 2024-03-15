#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <ncurses.h>
#include <unistd.h>
#define MAPWIDTH 80
#define MAPHEIGHT 21
#include "heap.h"

/*
USE raw(); to not enter after input
noecho() to not print character input
curs_set(0) no print cursor
start_color()
get color pairs for curses
*/

//% = border, boulder, mountain
// ^ = tree and forest
// # = road
// C and M = marts
// : = long grass
// . = clearing in grass
// ~ = water
// @ = player
// letter = npc
typedef enum Tiles                                                                   
{
    TREE,  ROCK,  ROAD, LONG,SHORT,WATER,MART,CENTER, TILE_MAX                                                                                                                
}tiles;   
 char * const tile_str[] =
{
    // [TREE]  = "\033[42;37m^\033[0m", 
	// [ROCK] = "\033[45;37m%\033[0m", 
	// [ROAD] = "\033[40;37m#\033[0m", 
    // [LONG] = "\033[102;37m:\033[0m", 
	// [SHORT] = "\033[102;37m.\033[0m", 
	// [WATER] = "\033[46;37m~\033[0m", 
	// [MART] = "\033[46;37mM\033[0m", 
	// [CENTER] = "\033[41;37mP\033[0m",
	[TREE]  = "^", 
	[ROCK] = "%", 
	[ROAD] = "#", 
    [LONG] = ":", 
	[SHORT] = ".", 
	[WATER] = "~", 
	[MART] = "M", 
	[CENTER] = "P",
};
typedef enum Characters{
	PLAYER, HIKER, RIVAL, PACER, WANDERER, SENTRIES, EXPLORERS
}characters;
char * const character_str[]={
	// [PLAYER] = "\033[044;05m@\033[0m",
	// [HIKER] = "\033[44;05mh\033[0m",
	// [RIVAL] = "\033[44;05mr\033[0m",
	// [PACER] = "\033[44;05mp\033[0m",
	// [WANDERER] = "\033[44;05mw\033[0m",
	// [SENTRIES] = "\033[44;05ms\033[0m",
	// [EXPLORERS] = "\033[44;05me\033[0m"
	[PLAYER] = "@",
	[HIKER] = "h",
	[RIVAL] = "r",
	[PACER] = "p",
	[WANDERER] = "w",
	[SENTRIES] = "s",
	[EXPLORERS] = "e"
};
enum Commands{
	EMPTY,
	N,
	E,
	S,
	W,
	FLY,
	Q,
	NUMTRAINERS
};
char * const command_str[]={
	[EMPTY] = "",
	[N] = "N",
	[S] = "S",
	[E] = "E",
	[W] = "W",
	[FLY] = "F",
	[Q] = "Q",
	[NUMTRAINERS] = "--numtrainers"
};

typedef struct character{
	heap_node_t *hn;
	int posX;
	int posY;
	int nextX;
	int nextY;
	char *symbol;
	bool tileLocked;
	int dir; //0 = left 1 = right 2 = up 3 = down
	char *tile;
	int nextTurn;
	int sequenceNum;
}character_c;
typedef struct map{
	bool generated;
	int gateAx;//top
	int gateBx;//bottom
	int gateCy;//left
	int gateDy;//right
	char *tiles[MAPHEIGHT][MAPWIDTH];
	character_c chars[MAPHEIGHT][MAPWIDTH];
	int hikerMap[MAPHEIGHT][MAPWIDTH];
	int rivalMap[MAPHEIGHT][MAPWIDTH];
	//int *otherCostMap[MAPHEIGHT][MAPWIDTH]; same as rivial for the time being
}map;
typedef struct globe{
	map *maps[401][401];
	int playerX;
	int playerY;
}globe_m;


typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;

//messy global vars
globe_m globe;
//check left, right, top, bottom, upleft ,downleft, upright, downright
int dirX[] = {-1,1,0, 0, -1,-1,1,1};
int dirY[] = {0 ,0,1,-1,  1,-1,1,0};
int posx=200,posy=200;
int playerx, playery;
map *currentMap;
char *oldMap[MAPHEIGHT][MAPWIDTH];
int heightmap[MAPHEIGHT][MAPWIDTH];
character_c player;
character_c hiker;
character_c rival;
character_c pacer;
character_c wanderer;
character_c sentery;
character_c explorer;
int numTrainers = 10;
int currentTime = 0;
int DEBUGCHANGENUM = 0;
//messy function defs
void initMap(int a, int b, int c, int d);
void printMap(map *Map);
void genPaths(int a, int b, int c, int d);
void genBuildings();
void loadMap();
map* createMap();
void copyMap(map* destination, const map* source);
void calcCost(int type,map *Map);//0 = hiker 1 = rival
void handleNPC(character_c chars[MAPHEIGHT][MAPWIDTH]);
void placeNPC();
int getTileCost(char *tile,int type);
static int32_t char_cmp(const void *key, const void *with);
heap_t charHeap;

int main(int argc, char *argv[]){
	

	initscr();			/* Start curses mode 		  */
	noecho();
	curs_set(0);
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(6, COLOR_WHITE, COLOR_CYAN);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
	init_pair(8, COLOR_WHITE, COLOR_BLUE);
	init_pair(9, COLOR_WHITE, COLOR_RED);
	init_pair(10, COLOR_WHITE, COLOR_GREEN);


	//printw("Hello World !!!");	/* Print Hello World		  */
	//refresh();			/* Print it on to the real screen */
	// getch();			/* Wait for user input */
	// endwin();			/* End curses mode		  */

	heap_init(&charHeap,char_cmp,NULL);
	char command[20];
	//bool numTrainerSwitch = false;
	for (int i = 1; i < argc; i++) {
		if(!strcmp(argv[i],command_str[NUMTRAINERS])){
	//		numTrainerSwitch = true;
			if(i < argc){
				numTrainers = atoi(argv[i+1]);
			}
		}
    }
	//printf("TRAINERS SET TO: %d, %d\n",numTrainers,numTrainerSwitch);
	while (*command != 'Q')
	{	
		printf("\e[1;1H\e[2J"); //CLEAR SCREEN PRINT
	    srand ( time(NULL) );
		if(globe.maps[posy][posx] == NULL){
			globe.maps[posy][posx] = createMap();
			currentMap = createMap();
		}
		//printf("command: %s\n",command);
		if(!globe.maps[posy][posx]->generated){
			//printf("first init");
			initMap(99,99,99,99);
			//handleNPC(currentMap->chars);
		}else{
			loadMap();
		}
		//calcCost(0);
		//calcCost(1);
		usleep(250000);
		DEBUGCHANGENUM++;
		continue;
		//break;
		printf("\033[96mEnter command: \033[0m");
		//scanf("%s",command);
		//fgets(command,20,stdin);
		char *s = command;
		 while (*s) {
    		*s = toupper((unsigned char) *s);
   		 	s++;
  		}
		s[strlen(s)-1] = '\0'; //get rid of newline from the fgets()
		char copyStr[20];
		strcpy(copyStr,command);
		if(strcmp(command,command_str[EMPTY])){
			//globe.maps[posy][posx] = currentMap;
			if(!strcmp(command,command_str[Q])){
				break;
			}else if(!strcmp(command,command_str[N])){
				if(posy < 401){
					posy--;	
				}
				loadMap();
			}else if(!strcmp(command,command_str[S])){
				if(posy>=0)
					posy++;
				loadMap();
			}else if(!strcmp(command,command_str[E])){
				if(posx<401)
					posx++;
				loadMap();
			}else if(!strcmp(command,command_str[W])){
				if(posx>=0)
					posx--;
				loadMap();
			}else if(!strcmp(strtok(copyStr, " "),command_str[FLY])){
				int i,x,y;
				for (i = 0; command[i] != '\0'; ++i)
					;
				if(i > 11|| i < 3){
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
	if(globe.maps[posy][posx] == NULL){
		//printf("in loadmap");
		//LOGIC FOR CONNECTING GATES BETWEEN MAPS VVVVVVV
		int a=99,b=99,c=99,d=99;

		if(posy > 0){
			if(globe.maps[posy-1][posx] != NULL){
				if (globe.maps[posy-1][posx]->gateBx != 99){
					a= globe.maps[posy-1][posx]->gateBx;
				}
			}
		}
		if(posy < 400){
			if(globe.maps[posy+1][posx] != NULL){
				if (globe.maps[posy+1][posx]->gateAx != 99){
					b = globe.maps[posy+1][posx]->gateAx;
				}		
			}
		}
		if(posx < 400){
			if(globe.maps[posy][posx+1] != NULL){
				if (globe.maps[posy][posx+1]->gateCy != 99){
					d = globe.maps[posy][posx+1]->gateCy;
				}
			}
		}
		if(posx > 0){
			if(globe.maps[posy][posx-1]!=NULL){
				if (globe.maps[posy][posx-1]->gateDy != 99){
					c = globe.maps[posy][posx-1]->gateDy;
				}
			}
		}

		//globe.maps[posy][posx] = createMap();	
		initMap(a,b,c,d);
	}else{
		//printf("\nFOUND MAP\n");
		currentMap = globe.maps[posy][posx];
		handleNPC(currentMap->chars);
		printMap( globe.maps[posy][posx]);


	}


}
void initMap(int a,int b,int c, int d){
	//printf("INIT\n");
	//to get random rand seed
	//free(currentMap);
	//currentMap = createMap();
	int i,j;

	int tallGrass = 0,shortGrass = 0,water =0,tree = 0,rock=0;
	for(i = 0; i < MAPHEIGHT; i++){
		for(j= 0; j < MAPWIDTH; j++){
			if(i == 0 || i == MAPHEIGHT - 1 || j == 0 || j == MAPWIDTH -1){
				oldMap[i][j] = tile_str[ROCK];
				heightmap[i][j] = 100;
			}else{
				oldMap[i][j] = "-";
				heightmap[i][j] = 0;
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
			heightmap[x][y] = 10;
			tallGrass++;
			continue;
		}
		if(shortGrass < 3){
			oldMap[x][y] = tile_str[SHORT];
			heightmap[x][y] = 5;
			shortGrass++;
			continue;
		}
		if(water < 1){
			oldMap[x][y] = tile_str[WATER];
			heightmap[x][y] = 25;
			water++;
			continue;
		}
		if(rock < 1){
			oldMap[x][y] = tile_str[ROCK];
			heightmap[x][y] = 20;
			rock++;
			continue;
		}
		if(tree < 3){
			oldMap[x][y] = tile_str[TREE];
			heightmap[x][y] = 15;
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
		//int weight = rand() % 10;
		if(pos > 6){
		//	weight *= -1;
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
							heightmap[i-1][j] = heightmap[i][j];
						}
						if(*oldMap[i+1][j]=='-'){
							temp[i+1][j] = oldMap[i][j];
							heightmap[i+1][j] = heightmap[i][j];
						}
					}else{
						if(*oldMap[i][j-1]=='-'){
							temp[i][j-1] = oldMap[i][j];
							heightmap[i][j-1] = heightmap[i][j];
						}
						if(*oldMap[i][j+1]== '-'){
							temp[i][j+1] = oldMap[i][j];
							heightmap[i][j+1] = heightmap[i][j];
						}
					}
				}
			}			
		}
		//copy temp back to oldMap
		memcpy(&oldMap,&temp,sizeof oldMap);
		if(!foundEmpty){growing=false;}
		foundEmpty = false;
	}
	
	//printf("before path a%d b%d c%d d%d \n",a,b,c,d);
	genPaths(a,b,c,d);
	//printf("\nafter path\n");
	genBuildings();
	//temp player thing here
	bool genPlayer = false;
	while(!genPlayer){
		int playerX = rand() % 70; playerX+=5;
		int playerY = rand() % 15; playerY+=2;

		if(oldMap[playerY][playerX] == tile_str[ROAD]){
			player.symbol = character_str[PLAYER];
			player.posX = playerX;
			player.posY = playerY;
			globe.playerX = playerX;
			globe.playerY = playerY;
			player.nextTurn = getTileCost(oldMap[playerY][playerX],99);
			currentMap->chars[playerY][playerX] = player;
			currentMap->chars[playerY][playerX].hn = 
			heap_insert(&charHeap, &currentMap->chars[playerY][playerX]);
			//oldMap[playerY][playerX] = character_str[PLAYER];
			genPlayer = true;
			//printf("PLAYER POS: %d, %d \n",player.posX,player.posY);

		}
	}
	for (i = 0;i < MAPHEIGHT;i++){
		for(j=0;j <MAPWIDTH;j++){
			currentMap->tiles[i][j] = oldMap[i][j];
		}
	} 
	calcCost(0,currentMap);calcCost(1,currentMap);
	placeNPC();
	printMap(currentMap);
	copyMap(globe.maps[posy][posx],currentMap);
	free(currentMap);
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
				if(heightmap[y+1][x] < heightmap[y][x]+3){
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
				if(heightmap[y-1][x] < heightmap[y][x] + 3){
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
				if(heightmap[y-1][x] < heightmap[y][x]){
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
				if(heightmap[y+1][x] < heightmap[y][x]){
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
			//printf("force out");
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
		//top dont gen a	ctob btod
		x = 0;y=c;
		targetx =b; targety = MAPHEIGHT -2;
		findPath(x,y,targetx,targety);
		x = b; y = MAPHEIGHT -1;	
		targetx =MAPWIDTH -2; targety = d;
		findPath(x,y,targetx,targety);
		oldMap[MAPHEIGHT-1][b] = tile_str[ROAD];
		oldMap[c][0]= tile_str[ROAD];
		oldMap[d][MAPWIDTH -1] = tile_str[ROAD];
		currentMap->gateAx=99;
		currentMap->gateBx=b;
		currentMap->gateCy=c;
		currentMap->gateDy=d;

	}else if(posy == 400){
		//bottom dont gen b ctoa atod
		x = a; y = 0;
		targetx = MAPWIDTH -2; targety = d;
		findPath(x,y,targetx,targety);
		x = 0;y = c;
		targetx = a; targety = 1;
		findPath(x,y,targetx,targety);
		oldMap[0][a] = tile_str[ROAD];
		oldMap[c][0]= tile_str[ROAD];
		oldMap[d][MAPWIDTH -1] = tile_str[ROAD];
		currentMap->gateAx=a;
		currentMap->gateBx=99;
		currentMap->gateCy=c;
		currentMap->gateDy=d;
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
					}else if(heightmap[y][x+1] < heightmap[y][x]){
						x++;
					}else if(heightmap[y+1][x+1] < heightmap[y][x]){
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
					}else if(heightmap[y][x-1] < heightmap[y][x]){
						x--;
					}else if(heightmap[y+1][x-1] < heightmap[y][x]){
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
				}else if(heightmap[y][x+1] < heightmap[y][x]){
					x++;
				}else if(heightmap[y+1][x+1] < heightmap[y][x] && targety != y){
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
				}else if(heightmap[y][x+1] < heightmap[y][x]){
					x++;
				}else if(heightmap[y-1][x+1] < heightmap[y][x] && targety != y){
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

	double distance = sqrt(pow(posx-200,2)+ pow(posy-200,2));
	double martchance = ((-25 * distance)/200)+50;
	double chance = rand() % 100;
	//printf("Distance: %f chance: %f rand: %f\n",distance,martchance,chance);
	if(chance > martchance){
		//printf("no mart");
		mart = true;
	}
	chance = rand() % 100;
	if(chance > martchance){
		//printf("no center\n");
		center = true;
	}
	while(!mart || !center){
		//printf("building loop %d %d\n",mart,center);
		int spot = 0;
		x = rand() % 76; x += 2;
		y = rand() % 17; y += 3;
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
	clear();
	//printw("\n");
	attron(COLOR_PAIR(1));
	printw("CURRENT TIME: %d, turn: %d\n",currentTime,DEBUGCHANGENUM);
	attroff(COLOR_PAIR(1));
	int i,j;
	for(i = 0; i < MAPHEIGHT; i++)
		for(j= 0; j < MAPWIDTH; j++){
			if(Map->chars[i][j].symbol != NULL){
				if(!strcasecmp(character_str[PLAYER],Map->chars[i][j].symbol)){
						attron(COLOR_PAIR(6));
						attron(A_BLINK);
						printw("%s",Map->chars[i][j].symbol);
						attroff(COLOR_PAIR(6));
						attroff(A_BLINK);
				}else{
					attron(COLOR_PAIR(2));
					printw("%s",Map->chars[i][j].symbol);
					attroff(COLOR_PAIR(2));
				}
			}else{
				if(!strcmp(tile_str[TREE],Map->tiles[i][j])){
					attron(COLOR_PAIR(10));
				}else if(!strcmp(tile_str[ROCK],Map->tiles[i][j])){
					attron(COLOR_PAIR(5));
				}else if(!strcmp(tile_str[ROAD],Map->tiles[i][j])){
					attron(COLOR_PAIR(7));
				}else if(!strcmp(tile_str[LONG],Map->tiles[i][j])){
					attron(COLOR_PAIR(3));
				}else if(!strcmp(tile_str[SHORT],Map->tiles[i][j])){
					attron(COLOR_PAIR(3));
				}else if(!strcmp(tile_str[WATER],Map->tiles[i][j])){
					attron(COLOR_PAIR(6));
				}else if(!strcmp(tile_str[MART],Map->tiles[i][j])){
					attron(COLOR_PAIR(8));
				}else if(!strcmp(tile_str[CENTER],Map->tiles[i][j])){
					attron(COLOR_PAIR(9));
				}
				printw("%s",Map->tiles[i][j]);
					attroff(COLOR_PAIR(1));
					attroff(COLOR_PAIR(2));
					attroff(COLOR_PAIR(3));
					attroff(COLOR_PAIR(4));
					attroff(COLOR_PAIR(5));
					attroff(COLOR_PAIR(6));
					attroff(COLOR_PAIR(7));
					attroff(COLOR_PAIR(8));
					attroff(COLOR_PAIR(9));
					attroff(COLOR_PAIR(10));
			}
			if(j == MAPWIDTH -1){
				printw("\n");
			}
		}
	mvaddstr(15,25,"stuff printed to middle of screen");
	mvaddstr(20,5,"RAHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH");
  	mvaddstr(21,5,"line 21");
  	mvaddstr(22,5,"line 22");
	mvaddstr(23,5,"line 23");
	refresh();
	// for(i = 0; i < MAPHEIGHT; i++){
	// 	for(j= 0; j < MAPWIDTH; j++) {			
	// 		if(j == globe.playerX && i == globe.playerY){
	// 			printf("\033[41;37m%02d\033[0m",Map->hikerMap[i][j]%100);
	// 		}else if(Map->hikerMap[i][j] >= INT16_MAX){
	// 			printf("   ");

	// 		}else{
	// 			printf("%02d ",Map->hikerMap[i][j]%100);
	// 		}
	// 		if(j == MAPWIDTH -1){
	// 			printf("\n");
	// 		}
	// 	}
	// }
	// for(i = 0; i < MAPHEIGHT; i++){
	// 	for(j= 0; j < MAPWIDTH; j++){			
	// 		if(j == globe.playerX && i == globe.playerY){
	// 			printfm("\033[41;37m%02d\033[0m",Map->rivalMap[i][j]%100);
	// 		}else if(Map->rivalMap[i][j] >= INT16_MAX){
	// 			printf("   ");

	// 		}else{
	// 			printf("%02d ",Map->rivalMap[i][j]%100);
	// 		}
	// 		if(j == MAPWIDTH -1){
	// 			printf("\n");
	// 		}
	// 	}
	// }
	//printf("\033[0mCurrent Map Pos = ");
	//printf("(%d,%d)\n",posx-200,posy-200);
	//printf("(%d,%d)\n",posx,posy);
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
			destination->chars[i][j] = source->chars[i][j];
			destination->hikerMap[i][j] = source->hikerMap[i][j];
			destination->rivalMap[i][j] = source->rivalMap[i][j];
        }
    }

}

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

int getTileCost(char *tile,int type){
	if(type == 0){
		if(!strcmp(tile,tile_str[ROAD])){
			//printf("r");
			return 10;
		}
			if(!strcmp(tile,tile_str[MART])){
			//printf("m");
			return 50;
		}
			if(!strcmp(tile,tile_str[CENTER])){
			//	printf("c");
			return 50;
		}
			if(!strcmp(tile,tile_str[LONG])){
			//	printf("l");
			return 15;
		}
			if(!strcmp(tile,tile_str[SHORT])){
			//	printf("s");
			return 10;
		}
			if(!strcmp(tile,tile_str[ROCK])){
			//	printf("r");
			return 15;
		}
			if(!strcmp(tile,tile_str[TREE])){
			//	printf("t");
			return 15;
		}
			if(!strcmp(tile,tile_str[WATER])){
			return INT16_MAX;
		}
	}else if(type == 1){//rival
		if(!strcmp(tile,tile_str[ROAD])){
		//printf("r");
		return 10;
		}
		if(!strcmp(tile,tile_str[MART])){
		//printf("m");
		return 50;
		}
		if(!strcmp(tile,tile_str[CENTER])){
		//	printf("c");
		return 50;
		}
		if(!strcmp(tile,tile_str[LONG])){
		//	printf("l");
		return 20;
		}
		if(!strcmp(tile,tile_str[SHORT])){
		//	printf("s");
		return 10;
		}
		if(!strcmp(tile,tile_str[ROCK])){
		//	printf("r");
		return INT16_MAX;
		}
		if(!strcmp(tile,tile_str[TREE])){
		return INT16_MAX;
		}
		if(!strcmp(tile,tile_str[WATER])){
		return INT16_MAX;
		}
	}else if(type == 99){//player
		if(!strcmp(tile,tile_str[ROAD])){
		//printf("r");
		return 10;
		}
		if(!strcmp(tile,tile_str[MART])){
		//printf("m");
		return 10;
		}
		if(!strcmp(tile,tile_str[CENTER])){
		//	printf("c");
		return 10;
		}
		if(!strcmp(tile,tile_str[LONG])){
		//	printf("l");
		return 20;
		}
		if(!strcmp(tile,tile_str[SHORT])){
		//	printf("s");
		return 10;
		}
		if(!strcmp(tile,tile_str[ROCK])){
		//	printf("r");
		return INT16_MAX;
		}
		if(!strcmp(tile,tile_str[TREE])){
		return INT16_MAX;
		}
		if(!strcmp(tile,tile_str[WATER])){
		return INT16_MAX;
		}
	}else{
		if(!strcmp(tile,tile_str[ROAD])){
		//printf("r");
		return 10;
		}
		if(!strcmp(tile,tile_str[MART])){
		//printf("m");
		return 50;
		}
		if(!strcmp(tile,tile_str[CENTER])){
		//	printf("c");
		return 50;
		}
		if(!strcmp(tile,tile_str[LONG])){
		//	printf("l");
		return 20;
		}
		if(!strcmp(tile,tile_str[SHORT])){
		//	printf("s");
		return 10;
		}
		if(!strcmp(tile,tile_str[ROCK])){
		//	printf("r");
		return INT16_MAX;
		}
		if(!strcmp(tile,tile_str[TREE])){
		return INT16_MAX;
		}
		if(!strcmp(tile,tile_str[WATER])){
		return INT16_MAX;
		}
	}
	return 0;
}

void calcCost(int type, map *Map){
	path_t path[MAPHEIGHT][MAPWIDTH],*p;	
  	uint32_t initialized = 0;
	heap_t h;
	uint32_t x, y;

	//printf("%d\n",type);
	if (!initialized) {
		for (y = 0; y < MAPHEIGHT; y++) {
			for (x = 0; x < MAPWIDTH; x++) {
				path[y][x].pos[1] = y;
				path[y][x].pos[0] = x;
				path[y][x].cost = INT16_MAX;
			}
		}
		initialized = 1;
		}	
	path[globe.playerY][globe.playerX].cost = 0;
	
	heap_init(&h, path_cmp, NULL);

	for (y = 1; y < MAPHEIGHT - 1; y++) {
	    for (x = 1; x < MAPWIDTH - 1; x++) {
    	  	path[y][x].hn = heap_insert(&h, &path[y][x]);
			}
	}

	 while ((p = heap_remove_min(&h))) {
  		p->hn = NULL;
		//printf("P COST : %d y: %d x: %d\n",p->cost,p->pos[1], p->pos[0]);
		int alt;

		if(p->pos[1] > 1 && p->pos[1] < 19 && p->pos[0] > 1 && p->pos[0] < 78){
			//up
			alt = p->cost + getTileCost(Map->tiles[p->pos[1]-1][p->pos[0]],type);
			if(alt < path[p->pos[1]-1][p->pos[0]].cost){
				path[p->pos[1]-1][p->pos[0]].cost = alt;
				//printf("beep 1 %d \n",alt);
				heap_decrease_key_no_replace(&h,path[p->pos[1]-1][p->pos[0]].hn);
			}
			//upright
			alt = p->cost + getTileCost(Map->tiles[p->pos[1]-1][p->pos[0]+1],type);
			if(alt < path[p->pos[1]-1][p->pos[0]+1].cost){
				path[p->pos[1]-1][p->pos[0]+1].cost = alt;
				//printf("beep 1 %d \n",alt);
				heap_decrease_key_no_replace(&h,path[p->pos[1]-1][p->pos[0]+1].hn);
			}
		//right
			alt = p->cost + getTileCost(Map->tiles[p->pos[1]][p->pos[0]+1],type);
			if(alt < path[p->pos[1]][p->pos[0]+1].cost){
				path[p->pos[1]][p->pos[0]+1].cost = alt;
			//	printf("beep 2 \n");
				heap_decrease_key_no_replace(&h,path[p->pos[1]][p->pos[0]+1].hn);
			}
			//rightdown
			alt = p->cost + getTileCost(Map->tiles[p->pos[1]+1][p->pos[0]+1],type);
			if(alt < path[p->pos[1]+1][p->pos[0]+1].cost){
				path[p->pos[1]+1][p->pos[0]+1].cost = alt;
			//	printf("beep 2 \n");
				heap_decrease_key_no_replace(&h,path[p->pos[1]+1][p->pos[0]+1].hn);
			}
		//down
			alt = p->cost + getTileCost(Map->tiles[p->pos[1]+1][p->pos[0]],type);	
			if(alt < path[p->pos[1]+1][p->pos[0]].cost){
				path[p->pos[1]+1][p->pos[0]].cost = alt;
				heap_decrease_key_no_replace(&h,path[p->pos[1]+1][p->pos[0]].hn);
			}
			//down left
			alt = p->cost + getTileCost(Map->tiles[p->pos[1]+1][p->pos[0]-1],type);
			if(alt < path[p->pos[1]+1][p->pos[0]-1].cost){
				path[p->pos[1]+1][p->pos[0]-1].cost = alt;
				heap_decrease_key_no_replace(&h,path[p->pos[1]+1][p->pos[0]-1].hn);
			}
		// //left
			alt = p->cost + getTileCost(Map->tiles[p->pos[1]][p->pos[0]-1],type);
			if(alt < path[p->pos[1]][p->pos[0]-1].cost){
				path[p->pos[1]][p->pos[0]-1].cost = alt;
				heap_decrease_key_no_replace(&h,path[p->pos[1]][p->pos[0]-1].hn);
			}
			//left up
			alt = p->cost + getTileCost(Map->tiles[p->pos[1]-1][p->pos[0]-1],type);
			if(alt < path[p->pos[1]-1][p->pos[0]-1].cost){
				path[p->pos[1]-1][p->pos[0]-1].cost = alt;
				heap_decrease_key_no_replace(&h,path[p->pos[1]-1][p->pos[0]-1].hn);
			}
		}
	}
	int i,j;
	//printf("\n");
	for(i = 0; i < MAPHEIGHT; i++){
		for(j= 0; j < MAPWIDTH; j++){
			if(type == 0){
				Map->hikerMap[i][j] = path[i][j].cost;
			}else{
				Map->rivalMap[i][j] = path[i][j].cost;
			}
		}
	}
}

static int32_t char_cmp(const void *key, const void *with) {
  return ((character_c *) key)->nextTurn - ((character_c *) with)->nextTurn;
}

void setNextPace(int nextY,int nextX,int posY, int posX,int dir,int type){
	int nextx = nextX;
	int nexty = nextY;
	char *currentTile = currentMap->chars[posY][posX].tile;
	if(dir == 0){//left
		if(currentMap->rivalMap[nextY][nextX - 1] != INT16_MAX
		&& currentMap->chars[nextY][nextX - 1].symbol == NULL){
			nextx = nextX-1;
		}else{
			if(type == 3 || type == 4)
				dir = rand()%4;
			else
				dir = 1;
		}
	}else if(dir == 1){//right
		if(currentMap->rivalMap[nextY][nextX + 1] != INT16_MAX
		&& currentMap->chars[nextY][nextX + 1].symbol == NULL){
			nextx = nextX+1;
		}else{
			if(type == 3 || type == 4)
				dir = rand()%4;
			else
				dir = 0;
		}
	}else if(dir == 2){//up
		if(currentMap->rivalMap[nextY-1][nextX] != INT16_MAX
		&& currentMap->chars[nextY-1][nextX].symbol == NULL){
			nexty = nextY-1;
		}else{
			if(type == 3 || type == 4)
				dir = rand()%4;
			else
				dir = 3;
		}
	}else if(dir == 3){//down
		if(currentMap->rivalMap[nextY+1][nextX] != INT16_MAX
		&& currentMap->chars[nextY+1][nextX].symbol == NULL){
			nexty = nextY+1;
		}else{
			if(type == 3 || type == 4)
				dir = rand()%4;
			else
				dir = 2;
		}
	}
	if(type == 3)
		currentMap->chars[nextY][nextX].tile = currentMap->chars[posY][posX].tile;

	if(type != 3 || !strcmp(currentTile,currentMap->tiles[nexty][nextx])){
		currentMap->chars[nextY][nextX].nextY = nexty;
		currentMap->chars[nextY][nextX].nextX = nextx;
		currentMap->chars[nextY][nextX].nextTurn = getTileCost(currentMap->tiles[nexty][nextx],0) + currentMap->chars[posY][posX].nextTurn;
	}else{
		dir = rand()%4;
		currentMap->chars[nextY][nextX].nextY = posY;
		currentMap->chars[nextY][nextX].nextX = posX;
		currentMap->chars[nextY][nextX].nextTurn = getTileCost(currentMap->tiles[posY][posX],0) + currentMap->chars[posY][posX].nextTurn;
	}
	currentMap->chars[nextY][nextX].dir = dir;

	
	//printf("PACER: Dir:%d,ny%d,nx%d,y%d,x%d\n",dir,nextY,nextX,posY,posX);
}
void setNextTurn(int posY,int posX,int type,int ox,int oy){
	int i;
	int min = 9999;
	int nextx = 0;
	int nexty = 0;
		for(i = 0; i < 8; i++){
		if(type == 0){
			if(posY +dirY[i] > 19 || posY+dirY[i] < 1 
			|| posX +dirX[i] > 79 || posX+dirX[i] < 1){
				continue;
			}
				//check surrounding for lowest cost
			if(currentMap->hikerMap[posY+dirY[i]][posX+dirX[i]] < min  && currentMap->chars[posY+dirY[i]][posX+dirX[i]].symbol == NULL){
				nextx = posX + dirX[i];
				nexty = posY + dirY[i];
				min = currentMap->hikerMap[nexty][nextx];
			}
		}else if(type==1){
			if(currentMap->rivalMap[posY+dirY[i]][posX+dirX[i]] < min  && currentMap->chars[posY+dirY[i]][posX+dirX[i]].symbol == NULL){
				nextx = posX + dirX[i];
				nexty = posY + dirY[i];
				min = currentMap->rivalMap[nexty][nextx];
			}
		}
	}

	if(nextx == 0){
			nextx = ox;
		}
		if(nexty == 0){
			nexty= oy;
		}	
		currentMap->chars[posY][posX].nextY = nexty;
		currentMap->chars[posY][posX].nextX = nextx;
	if(type == 0){			
		currentMap->chars[posY][posX].nextTurn = getTileCost(currentMap->tiles[nexty][nextx],0) + currentMap->chars[oy][ox].nextTurn;
	}else if (type == 1){//rivial and everything else 
		currentMap->chars[posY][posX].nextTurn = getTileCost(currentMap->tiles[nexty][nextx],1) + currentMap->chars[oy][ox].nextTurn;
	}else{
		currentMap->chars[posY][posX].nextTurn = getTileCost(currentMap->tiles[nexty][nextx],1) + currentMap->chars[oy][ox].nextTurn;
	}

}
void handleNPC(character_c chars[MAPHEIGHT][MAPWIDTH]){
	character_c *c;
	int timeNum = 0;
	while((c = heap_remove_min(&charHeap))){
		 if(c != NULL){
			if(timeNum == 0){
				timeNum = currentTime;
			}else if(c->nextTurn > timeNum){
				currentTime = currentMap->chars[c->posY][c->posX].nextTurn;
				heap_insert(&charHeap, &currentMap->chars[c->posY][c->posX]);
				//copyMap(globe.maps[posy][posx],currentMap);
				return;
			}
			// printf("FOUND INSERT\n");
			// printf("symbol:%s\n",c->symbol);
			// printf("x:%d\n",c->posX);
			// printf("y:%d\n",c->posY);
			// printf("next x:%d\n",c->nextX);
			// printf("next y:%d\n",c->nextY);
			// printf("next turn:%d\n",c->nextTurn);
			
			if(!strcmp(c->symbol,character_str[PLAYER])){
				//int num = rand() % 4;
				bool genPlayer = false;
				if(DEBUGCHANGENUM > 20){
					while(!genPlayer){
						int playerX = rand() % 70; playerX+=5;
						int playerY = rand() % 15; playerY+=2;
						if(!strcmp(currentMap->tiles[playerY][playerX],tile_str[ROAD])
						&& currentMap->chars[playerY][playerX].symbol == NULL){
							DEBUGCHANGENUM = 0;
							currentMap->chars[globe.playerY][globe.playerX].symbol = NULL;
							player.posX = playerX;
							player.posY = playerY;
							globe.playerX = playerX;
							globe.playerY = playerY;
							player.nextTurn += getTileCost(currentMap->tiles[playerY][playerX],99);
							currentMap->chars[playerY][playerX] = player;
							genPlayer = true;
						}
					}
					calcCost(0,currentMap);calcCost(1,currentMap);
					heap_insert(&charHeap, &currentMap->chars[globe.playerY][globe.playerX]);
				}else{
					currentMap->chars[globe.playerY][globe.playerX].nextTurn += getTileCost(currentMap->tiles[globe.playerY][globe.playerX],99);
					heap_insert(&charHeap, &currentMap->chars[globe.playerY][globe.playerX]);
				}
			}else if(!strcmp(c->symbol,character_str[HIKER])
			|| !strcmp(c->symbol,character_str[RIVAL])
			|| !strcmp(c->symbol,character_str[PACER])
			|| !strcmp(c->symbol,character_str[WANDERER])
			|| !strcmp(c->symbol,character_str[EXPLORERS])){
				if(currentMap->chars[c->nextY][c->nextX].symbol == NULL){
					currentMap->chars[c->nextY][c->nextX].symbol = currentMap->chars[c->posY][c->posX].symbol;
					currentMap->chars[c->nextY][c->nextX].posX = currentMap->chars[c->posY][c->posX].nextX;
					currentMap->chars[c->nextY][c->nextX].posY = currentMap->chars[c->posY][c->posX].nextY;

					if(!strcmp(c->symbol,character_str[HIKER])){
						setNextTurn(c->nextY,c->nextX,0,c->posX,c->posY);
					}else if (!strcmp(c->symbol,character_str[RIVAL])){
						setNextTurn(c->nextY,c->nextX,1,c->posX,c->posY);
					}else if (!strcmp(c->symbol,character_str[PACER])){
						setNextPace(c->nextY,c->nextX,c->posY,c->posX,c->dir,2);
					}else if (!strcmp(c->symbol,character_str[WANDERER])){
						setNextPace(c->nextY,c->nextX,c->posY,c->posX,c->dir,3);
					}else if (!strcmp(c->symbol,character_str[EXPLORERS])){
						setNextPace(c->nextY,c->nextX,c->posY,c->posX,c->dir,4);
					}
					// printf("NEW INSERT\n");
					// printf("symbol:%s\n",currentMap->chars[c->nextY][c->nextX].symbol);
					// printf("x:%d\n",currentMap->chars[c->nextY][c->nextX].posX);
					// printf("y:%d\n",currentMap->chars[c->nextY][c->nextX].posY);
					// printf("next x:%d\n",currentMap->chars[c->nextY][c->nextX].nextX);
					// printf("next y:%d\n",currentMap->chars[c->nextY][c->nextX].nextY);
					// printf("next turn:%d\n",currentMap->chars[c->nextY][c->nextX].nextTurn);
					currentTime = currentMap->chars[c->nextY][c->nextX].nextTurn;
					currentMap->chars[c->nextY][c->nextX].hn =
					heap_insert(&charHeap, &currentMap->chars[c->nextY][c->nextX]);

					currentMap->chars[c->posY][c->posX].symbol = NULL;
				}else{
					if(!strcmp(c->symbol,character_str[HIKER])){
						setNextTurn(c->posY,c->posX,0,c->posX,c->posY);
					}else if (!strcmp(c->symbol,character_str[RIVAL])){
						setNextTurn(c->posY,c->posX,1,c->posX,c->posY);
					}else if (!strcmp(c->symbol,character_str[PACER])){
						setNextPace(c->posY,c->posX,c->posY,c->posX,c->dir,2);
					}else if (!strcmp(c->symbol,character_str[WANDERER])){
						setNextPace(c->posY,c->posX,c->posY,c->posX,c->dir,3);
					}else if (!strcmp(c->symbol,character_str[EXPLORERS])){
						setNextPace(c->posY,c->posX,c->posY,c->posX,c->dir,4);
					}
					currentTime = currentMap->chars[c->posY][c->posX].nextTurn;
					heap_insert(&charHeap, &currentMap->chars[c->posY][c->posX]);
				}
			}
		}
	}
	//copyMap(globe.maps[posy][posx],currentMap);

}

void placeNPC(){		// weights of amount
	int numHiker=0, maxHiker = round(numTrainers * 0.10),			//25
	 numRival=0,	maxRival = round(numTrainers * 0.15), 			//25
	 numPacer=0,	maxPacer = round(numTrainers * 0.12), 			//10
	 numWanderer=0,	maxWanderer = round(numTrainers * 0.13), 		//10
	 numSentries=0,	maxSenteries = round(numTrainers * 0.08), 		//10
	 numExplorers=0,maxExplorers = round(numTrainers * 0.17), 		//20
	 //total=0,
	  totalMax = maxHiker + maxRival + maxPacer + maxWanderer+maxSenteries+maxExplorers;

	/*
		CHANGE WEIGHTS  I DONT LIKE IT ANYMORE
	*/
	if(numTrainers == 2){
		maxHiker = 1;
		maxRival = 1;
		totalMax = maxHiker + maxRival + maxPacer + maxWanderer+maxSenteries+maxExplorers;
	}else if(totalMax < numTrainers){
		maxHiker += numTrainers - totalMax;
		totalMax = maxHiker + maxRival + maxPacer + maxWanderer+maxSenteries+maxExplorers;
	}
	//printf("num npc, %d, %d, %d, %d ,%d ,%d, total: %d\n",numHiker,numRival,numPacer,numWanderer,numSentries,numExplorers,total);
	//printf("max npc, %d, %d, %d, %d ,%d ,%d, total: %d \n",maxHiker,maxRival,maxPacer,maxWanderer,maxSenteries,maxExplorers,
	//maxHiker + maxRival + maxPacer + maxWanderer+maxSenteries+maxExplorers);

	bool generated = false;
	//int uhOhTimer = 0;
	while(!generated){
		if(numHiker == maxHiker &&
		numRival == maxRival &&
		numPacer == maxPacer &&
		numSentries == maxSenteries &&
		numExplorers == maxExplorers &&
		numWanderer == maxWanderer){
			generated = true;
		}
		int x = rand() % 76; x += 2;
		int y = rand() % 17; y += 3;
		//printf("x: %d y: %d\n",x,y);
		if(!strcmp(currentMap->tiles[y][x],tile_str[TREE]) 
		|| !strcmp(currentMap->tiles[y][x],tile_str[WATER])
		|| !strcmp(currentMap->tiles[y][x],tile_str[ROCK])
		|| !strcmp(currentMap->tiles[y][x],tile_str[ROAD])){
			continue;
		}
		if(numHiker < maxHiker){
			if(currentMap->chars[y][x].symbol == NULL){
				hiker.symbol = character_str[HIKER];
				hiker.posX = x;
				hiker.posY = y;

				hiker.nextTurn = 0;//getTileCost(currentMap->tiles[hiker.nextY][hiker.nextX],0);
				numHiker++;
				currentMap->chars[y][x] = hiker;
				setNextTurn(y,x,0,x,y);
				//printf("CREATED HIKER, %s \n",currentMap->chars[y][x].symbol);
				currentMap->chars[y][x].hn = 
				heap_insert(&charHeap,&currentMap->chars[y][x]);
				
			}
		}
		if(numRival < maxRival){
				if(currentMap->chars[y][x].symbol == NULL){
				rival.symbol = character_str[RIVAL];
				rival.posX = x;
				rival.posY = y;
				rival.nextTurn = 0;//getTileCost(currentMap->tiles[rival.nextY][rival.nextX],0);
				numRival++;
				currentMap->chars[y][x] = rival;
				setNextTurn(y,x,1,x,y);
				//printf("CREATED RIVAL, %s \n",currentMap->chars[y][x].symbol);

				currentMap->chars[y][x].hn = 
				heap_insert(&charHeap,&currentMap->chars[y][x]);
			}
		
		}//still breaks sometimes
		if(numPacer < maxPacer){
			if(currentMap->chars[y][x].symbol == NULL){
				pacer.symbol = character_str[PACER];
				pacer.posX = x;
				pacer.posY = y;
				pacer.dir =	rand() %4;
				pacer.nextTurn = 0;
				currentMap->chars[y][x]= pacer;
				setNextPace(y,x,y,x,pacer.dir,2);
				numPacer++;
				heap_insert(&charHeap,&currentMap->chars[y][x]);
			}
			
		}
		if(numWanderer < maxWanderer){
			if(currentMap->chars[y][x].symbol == NULL){
				wanderer.symbol = character_str[WANDERER];
				wanderer.posX = x;
				wanderer.posY = y;
				wanderer.tile = currentMap->tiles[y][x];
				wanderer.dir = rand() %4;
				wanderer.nextTurn = 0;
				currentMap->chars[y][x]= wanderer;
				setNextPace(y,x,y,x,wanderer.dir,3);
				numWanderer++;
				heap_insert(&charHeap,&currentMap->chars[y][x]);
			}
			
		}
		if(numSentries < maxSenteries){
			if(currentMap->chars[y][x].symbol == NULL){
				sentery.symbol = character_str[SENTRIES];
				sentery.posX = x;
				sentery.posY = y;
				currentMap->chars[y][x]= sentery;
				numSentries++;
			}	
		}
		if(numExplorers < maxExplorers){
			if(currentMap->chars[y][x].symbol == NULL){
				explorer.symbol = character_str[EXPLORERS];
				explorer.posX = x;
				explorer.posY = y;
				explorer.dir = rand() %4;
				currentMap->chars[y][x]= explorer;
				numExplorers++;		
				setNextPace(y,x,y,x,explorer.dir,4);
				heap_insert(&charHeap,&currentMap->chars[y][x]);
			}
				
		}
	}
}
