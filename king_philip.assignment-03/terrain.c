#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define MAPWIDTH 80
#define MAPHEIGHT 21
#include "heap.h"

//MAKE THE COST MAPS PART OF GLOBAL MAPS STRUCT not local
//character struct local to map
/*
symbol
pos
next turn- cost of terrain next move would be - keep adding it do not replace
sequence num- used for tie breaker, order inserted into queue

desicrete event sim	
	event queue
	need new compare
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
    [TREE]  = "\033[43;37m^\033[0m", //weight 15 
	[ROCK] = "\033[45;37m%\033[0m", //weight is 20
	[ROAD] = "\033[40;37m#\033[0m", //weight is 0
    [LONG] = "\033[42;37m:\033[0m", //weight is 10
	[SHORT] = "\033[42;37m.\033[0m", //weight is 5
	[WATER] = "\033[46;37m~\033[0m", //weight is 25
	[MART] = "\033[46;37mM\033[0m", //weight is 100
	[CENTER] = "\033[41;37mP\033[0m", //weight is 100
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
// int hiker_c[]={
// 	[ROAD] = 10,
// 	[MART] = 50,
// 	[CENTER] = 50,
// 	[LONG] = 15,
// 	[SHORT] = 10,
// 	[ROCK] = 15,
// 	[TREE] = 15,
// 	[WATER] = INT16_MAX
// };
// int rival_c[]={
// 	[ROAD] = 10,
// 	[MART] = 50,
// 	[CENTER] = 50,
// 	[LONG] = 20,
// 	[SHORT] = 10,
// 	[ROCK] = 15,
// 	[TREE] = INT16_MAX,
// 	[WATER] = INT16_MAX,

// };

typedef struct player{
	int posX;
	int posY;
}player_c;

typedef struct map{
	bool generated;
	int gateAx;//top
	int gateBx;//bottom
	int gateCy;//left
	int gateDy;//right
	char *tiles[MAPHEIGHT][MAPWIDTH];
}map;

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;

map *maps[401][401];
int posx=200,posy=200;
map *currentMap;
char *oldMap[MAPHEIGHT][MAPWIDTH];
int heightmap[MAPHEIGHT][MAPWIDTH];

void initMap(int a, int b, int c, int d);
void printMap(map *Map);
void genPaths(int a, int b, int c, int d);
void genBuildings();
void loadMap();
map* createMap();
void copyMap(map* destination, const map* source);
void freeMap(map* mapToFree) ;
void calcCost(int type);//0 = hiker 1 = rival
player_c player;
int main(int argc, char *argv[]){
	char command[20] = " ";
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
		calcCost(0);
		calcCost(1);
		break;
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
	if(maps[posy][posx] == NULL){
		//printf("in loadmap");
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
		printMap( maps[posy][posx]);
		currentMap = maps[posy][posx];
	}


}
void initMap(int a,int b,int c, int d){
	//printf("INIT\n");
	//to get random rand seed

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
			oldMap[playerY][playerX] = "@";
			genPlayer = true;
			player.posX = playerX;
			player.posY = playerY;
			//printf("PLAYER POS: %d, %d \n",player.posX,player.posY);

		}
	}


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
			printf("%s",Map->tiles[i][j]);
			if(j == MAPWIDTH -1){
				printf("\n");
			}
		}
	}
	// for(i = 0; i < MAPHEIGHT; i++){
	// 	for(j= 0; j < MAPWIDTH; j++){
	// 		printf("%2d ",(int)heightmap[i][j] % 100);
	// 		if(j == MAPWIDTH -1){
	// 			printf("\n");
	// 		}
	// 	}
	// }
	//printf("\033[0mCurrent Map Pos = ");
	//printf("(%d,%d)\n",posx-200,posy-200);
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
		return 15;
		}
		if(!strcmp(tile,tile_str[TREE])){
		return INT16_MAX;
		}
		if(!strcmp(tile,tile_str[WATER])){
		return INT16_MAX;
		}
	}
	//printf("oop %s %d\n",tile,type);
	return 0;
}


void calcCost(int type){
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
	path[player.posY][player.posX].cost = 0;
	
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
			switch (type){
				case 0:
				break;
			}
			alt = p->cost + getTileCost(maps[posy][posx]->tiles[p->pos[1]-1][p->pos[0]],type);
			if(alt < path[p->pos[1]-1][p->pos[0]].cost){
				path[p->pos[1]-1][p->pos[0]].cost = alt;
				//printf("beep 1 %d \n",alt);
				heap_decrease_key_no_replace(&h,path[p->pos[1]-1][p->pos[0]].hn);
			}
			//upright
			alt = p->cost + getTileCost(maps[posy][posx]->tiles[p->pos[1]-1][p->pos[0]+1],type);
			if(alt < path[p->pos[1]-1][p->pos[0]+1].cost){
				path[p->pos[1]-1][p->pos[0]+1].cost = alt;
				//printf("beep 1 %d \n",alt);
				heap_decrease_key_no_replace(&h,path[p->pos[1]-1][p->pos[0]+1].hn);
			}
		//right
			alt = p->cost + getTileCost(maps[posy][posx]->tiles[p->pos[1]][p->pos[0]+1],type);
			if(alt < path[p->pos[1]][p->pos[0]+1].cost){
				path[p->pos[1]][p->pos[0]+1].cost = alt;
			//	printf("beep 2 \n");
				heap_decrease_key_no_replace(&h,path[p->pos[1]][p->pos[0]+1].hn);
			}
			//rightdown
			alt = p->cost + getTileCost(maps[posy][posx]->tiles[p->pos[1]+1][p->pos[0]+1],type);
			if(alt < path[p->pos[1]+1][p->pos[0]+1].cost){
				path[p->pos[1]+1][p->pos[0]+1].cost = alt;
			//	printf("beep 2 \n");
				heap_decrease_key_no_replace(&h,path[p->pos[1]+1][p->pos[0]+1].hn);
			}
		//down
			alt = p->cost + getTileCost(maps[posy][posx]->tiles[p->pos[1]+1][p->pos[0]],type);	
			if(alt < path[p->pos[1]+1][p->pos[0]].cost){
				path[p->pos[1]+1][p->pos[0]].cost = alt;
				heap_decrease_key_no_replace(&h,path[p->pos[1]+1][p->pos[0]].hn);
			}
			//down left
			alt = p->cost + getTileCost(maps[posy][posx]->tiles[p->pos[1]+1][p->pos[0]-1],type);
			if(alt < path[p->pos[1]+1][p->pos[0]-1].cost){
				path[p->pos[1]+1][p->pos[0]-1].cost = alt;
				heap_decrease_key_no_replace(&h,path[p->pos[1]+1][p->pos[0]-1].hn);
			}
		// //left
			alt = p->cost + getTileCost(maps[posy][posx]->tiles[p->pos[1]][p->pos[0]-1],type);
			if(alt < path[p->pos[1]][p->pos[0]-1].cost){
				path[p->pos[1]][p->pos[0]-1].cost = alt;
				heap_decrease_key_no_replace(&h,path[p->pos[1]][p->pos[0]-1].hn);
			}
			//left up
			alt = p->cost + getTileCost(maps[posy][posx]->tiles[p->pos[1]-1][p->pos[0]-1],type);
			if(alt < path[p->pos[1]-1][p->pos[0]-1].cost){
				path[p->pos[1]-1][p->pos[0]-1].cost = alt;
				heap_decrease_key_no_replace(&h,path[p->pos[1]-1][p->pos[0]-1].hn);
			}
		}
	}
	int i,j;
	printf("\n");
	for(i = 0; i < MAPHEIGHT; i++){
		for(j= 0; j < MAPWIDTH; j++){
			if(j == player.posX && i == player.posY){
				printf("\033[41;37m%02d\033[0m ",path[i][j].cost%100);
			}else if(path[i][j].cost >= INT16_MAX){
				printf("   ");

			}else{
				printf("%02d ",path[i][j].cost%100);
			}
			if(j == MAPWIDTH -1){
				printf("\n");
			}
		}
	}
}