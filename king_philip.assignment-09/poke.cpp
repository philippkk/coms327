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
#define MAPWIDTH 80
#define MAPHEIGHT 21
#define UIMIDOFFSET 15
#define KEY_ESC 27
#include "helpers/heap.h"
#include "helpers/character.h"
#include "helpers/pokeparser.h"




#define COLOR_PURPLE 8
#define COLOR_BROWN 9
#define COLOR_DARK_BROWN 10
#define COLOR_GRASS 11
#define COLOR_DARK_GRASS 12
#define COLOR_PATH 13
#define COLOR_DARK_PATH 14
#define COLOR_GREY 15
#define COLOR_HP 16

/* TODO

*/

using u32    = uint_least32_t; 
using engine = std::mt19937;
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
const char* tile_str[] =
{
	[TREE]  ="^", 
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
const char* character_str[]={
	[PLAYER] = "@",
	[HIKER] = "h",
	[RIVAL] = "r",
	[PACER] = "p",
	[WANDERER] = "w",
	[SENTRIES] = "s",
	[EXPLORERS] = "e"
};
class map{
	public:
		bool generated;
		int gateAx;//top
		int gateBx;//bottom
		int gateCy;//left
		int gateDy;//right
		char *tiles[MAPHEIGHT][MAPWIDTH];
		character_c chars[MAPHEIGHT][MAPWIDTH];
		int hikerMap[MAPHEIGHT][MAPWIDTH];
		int rivalMap[MAPHEIGHT][MAPWIDTH];
		heap_t localHeap;
	//int *otherCostMap[MAPHEIGHT][MAPWIDTH]; same as rivial for the time being
};
typedef class globe{
	public:
		map *maps[401][401];
		int playerX;
		int playerY;
		pokemonObject playerPokemon[6];
		pokemonObject *playerCurrentPokemon;
}globe_m;


typedef class path {
	public:
		heap_node_t *hn;
		uint8_t pos[2];
		uint8_t from[2];
		int32_t cost;
	path(){}
	path(void* input){
        uint8_t* dataPtr = static_cast<uint8_t*>(input);
        hn = reinterpret_cast<heap_node_t*>(dataPtr);
        for (int i = 0; i < 2; ++i) {
            pos[i] = *(dataPtr + sizeof(heap_node_t*) + i);
            from[i] = *(dataPtr + sizeof(heap_node_t*) + 2 + i);
        }
        cost = *reinterpret_cast<int32_t*>(dataPtr + sizeof(heap_node_t*) + 4);
	}
} path_t;

character_c player;
character_c *opponent;
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
int numTrainers = 10;
int currentTime = 0;
int DEBUGCHANGENUM = 0;
heap_t charHeap;
char command[20];
int selector =0,battleSelector=0;
int commandShort;
bool playerTurn;
bool inMart;
bool inCenter;
bool fly = false;
int flyX;
int flyY;
bool inBattle;
int bagSelector = 0;
bool inEcounter,showingOverworldbag,showingBag,showingSwitchPoke,showingMoves,playerBattleTurn = true;
bool wonBattle;
bool showingList;
bool showingPoke;
bool skipHeap = false;
int trainerListOffset = 0;

int debugInt = 0;

int revives=5,potions=5,pokeballs=5;
bool reviving = false;

std::vector<pokemon> pokemonDb;
std::vector<pokemon_moves> pokemon_movesDb;
std::vector<moves> movesDb;
std::vector<pokemon_stats> pokemon_statsDb;
std::vector<stats> statsDb;
std::vector<pokemon_types> pokemon_typesDb;
std::vector<type_names> type_namesDb;


std::string battleMessage = "empty";

pokemonObject encounteredPoke;

//messy function defs
void initMap(int a, int b, int c, int d);
void printMap(map *Map);
void genPaths(int a, int b, int c, int d);
void genBuildings();
void loadMap();
map* createMap();
void copyMap(map* destination, const map* source);
void playerReturnToMapCalc(int playerX, int playerY);
void calcCost(int type,map *Map);//0 = hiker 1 = rival
void handleNPC(character_c chars[MAPHEIGHT][MAPWIDTH]);
void placeNPC();
void handleBattle(character_c &trainer,int encounter);
pokemonObject createPokemon(bool isEncounter);
void levelUpPokemon(pokemonObject poke);
int getTileCost(char *tile,int type);
static int32_t char_cmp(const void *key, const void *with);

std::random_device os_seed;
const u32 seed = os_seed();
engine generator( seed );
std::uniform_int_distribution< u32 > xdistribute( 1, 79 );
std::uniform_int_distribution< u32 > ydistribute( 1, 20);
std::uniform_int_distribution< u32 > encounter( 0, 100);
int main(int argc, char *argv[]){

	globe.playerX = -9999;
	globe.playerY = -9999;
	globe.playerPokemon[0] = pokemonObject();
    globe.playerPokemon[1] = pokemonObject();
	globe.playerPokemon[2] = pokemonObject();
	globe.playerPokemon[3] = pokemonObject();
	globe.playerPokemon[4] = pokemonObject();
	globe.playerPokemon[5] = pokemonObject();
	globe.playerCurrentPokemon = &globe.playerPokemon[0];
	commandShort =  ' ';
	playerTurn = false;
	pokeparser parser = pokeparser();
	pokemonDb = parser.parsePokemon();
	movesDb = parser.parseMoves();
	pokemon_statsDb = parser.parsePokemonStats();
	statsDb = parser.parseStats();
	pokemon_typesDb = parser.parsePokemonTypes();
	type_namesDb = parser.parseTypeNames();
	std::cout << "loading..."<<std::endl;
	pokemon_movesDb = parser.parsePokemonMoves();
	std::cout << "done"<<std::endl;
	usleep(500000);
	initscr();			/* Start curses mode 		  */
	noecho();
	raw();
	curs_set(0);
	keypad(stdscr, TRUE);
	start_color();

	init_color(COLOR_PURPLE,990,495,0);
	init_color(COLOR_BROWN,661,537,367);
	init_color(COLOR_DARK_BROWN,461,337,167);
	init_color(COLOR_GRASS,151,524,179);
	init_color(COLOR_DARK_GRASS,51,424,79);
	init_color(COLOR_PATH,300,270,240);
	init_color(COLOR_DARK_PATH,200,170,140);
	init_color(COLOR_GREY,50,50,50);
	init_color(COLOR_HP,900,350,350);
	init_pair(1, COLOR_PURPLE, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	init_pair(5, COLOR_BROWN, COLOR_DARK_BROWN);
	init_pair(6, COLOR_WHITE, COLOR_CYAN);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
	init_pair(8, COLOR_WHITE, COLOR_BLUE);
	init_pair(9, COLOR_WHITE, COLOR_RED);
	init_pair(10, COLOR_WHITE, COLOR_GREEN);
	init_pair(11, COLOR_WHITE, COLOR_RED);
	init_pair(12, COLOR_WHITE, COLOR_BLACK);
	init_pair(13, COLOR_GRASS, COLOR_DARK_GRASS);
	init_pair(14, COLOR_DARK_GRASS, COLOR_GRASS);
	init_pair(15, COLOR_WHITE, COLOR_DARK_PATH);
	init_pair(16, COLOR_WHITE, COLOR_GREY);
	init_pair(17, COLOR_HP, COLOR_GREY);
	init_pair(18, COLOR_WHITE, COLOR_HP);


	heap_init(&charHeap,char_cmp,NULL);



	//use this for prase check
	for (int i = 1; i < argc; i++) {
		if(!strcmp(argv[i],"--numtrainers")){
	//		numTrainerSwitch = true;
			if(i < argc){
				numTrainers = atoi(argv[i+1]);
			}
		}
    }

	while (*command != 'Q')
	{	
		refresh();
	    srand ( time(NULL) );
		loadMap();
		if(playerTurn){
			playerTurn = false;
		}	
		if(commandShort == 'Q'){
			break;
		}
		//usleep(250000);
		DEBUGCHANGENUM++;
		continue;
	 }
	endwin();
	return 0;
}
void loadMap(){
	//printf("(%d,%d)pre\n",posx-200,posy-200);
		//printf("(%d,%d)pre real:\n",posx,posy);
	if(globe.maps[posy][posx] == NULL){
			globe.maps[posy][posx] = createMap();
			currentMap = createMap();
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
		printMap(currentMap);
		if(skipHeap){
			skipHeap = false;
			return;
		}
		handleNPC(globe.maps[posy][posx]->chars);
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
				oldMap[i][j]=(char *)"%";
				heightmap[i][j] = 100;
			}else{
				oldMap[i][j] = (char *)"-";
				heightmap[i][j] = 0;
			}
		}
	}
	//printf("after loop");
	bool seeded = false;
	//seeding here 
	while(!seeded){
		int x,y;
		//printf("seed");
		x = ydistribute(generator);//1-20
		y = xdistribute(generator);//1-79
		if(strcmp(oldMap[x][y],"-")){
			continue;
		}
		if(tallGrass < 3){
			oldMap[x][y] = (char *)tile_str[LONG];
			heightmap[x][y] = 10;
			tallGrass++;
			continue;
		}
		if(shortGrass < 3){
			oldMap[x][y] = (char *)tile_str[SHORT];
			heightmap[x][y] = 5;
			shortGrass++;
			continue;
		}
		if(water < 1){
			oldMap[x][y] = (char *)tile_str[WATER];
			heightmap[x][y] = 25;
			water++;
			continue;
		}
		if(rock < 1){
			oldMap[x][y] = (char *)tile_str[ROCK];
			heightmap[x][y] = 20;
			rock++;
			continue;
		}
		if(tree < 3){
			oldMap[x][y] = (char *)tile_str[TREE];
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
				if(!strcmp(oldMap[i][j], "-")){
					foundEmpty = true;
				}

				if(strcmp(oldMap[i][j], "-")){
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
		int playerX,playerY;
		if(globe.playerX != -9999){
			playerX = globe.playerX;
		}else{
			playerX = rand() % 70; playerX+=5;		
		}if(globe.playerY != -9999){
			playerY = globe.playerY;
		}else{
			playerY = rand() % 15; playerY+=2;
		}

		if(!strcmp(oldMap[playerY][playerX], tile_str[ROAD])){
			player.symbol = (char *)character_str[PLAYER];
			player.posX = playerX;
			player.posY = playerY;
			globe.playerX = playerX;
			globe.playerY = playerY;
			player.nextTurn = getTileCost(oldMap[playerY][playerX],99);
			currentMap->chars[playerY][playerX] = player;
			currentMap->chars[playerY][playerX].hn = 
			heap_insert(&currentMap->localHeap, &currentMap->chars[playerY][playerX]);
			genPlayer = true;
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

	
	pokemonObject poke1 = createPokemon(true);
	pokemonObject poke2 = createPokemon(true);
	pokemonObject poke3 = createPokemon(true);
	pokemonObject pokes[3] = {poke1,poke2,poke3};
	while(globe.playerPokemon[0].name == "empty"){
		mvaddch(4,15,  ACS_ULCORNER); 
		for(int i = 16; i <64;i++){
			mvaddch(4,i,ACS_HLINE);
		}
		mvaddch(4,64,  ACS_URCORNER); 
		mvaddch(5,15,  ACS_VLINE); 
		mvaddstr(5,16, "                 PICK A POKEMON                  ");
		mvaddch(5,64,  ACS_VLINE); 
		mvaddch(6,15,  ACS_LTEE); 
		for(int i = 16; i <64;i++){
			mvaddch(6,i,ACS_HLINE);
		}
		mvaddch(6,64,  ACS_RTEE); 
		for(int i = 7; i < 18;i++){
			mvaddch(i,15,  ACS_VLINE); 
			mvaddstr(i,16,"                                                ");
			mvaddch(i,64, ACS_VLINE);
		}
		for(int i = 16; i <64;i++){
			mvaddch(18,i,ACS_HLINE);
		}
		mvaddch(18,15,ACS_LLCORNER);
		mvaddch(18,64,ACS_LRCORNER);
		attron(COLOR_PAIR(11));
		for(int i = 0; i < 3; i++){
			mvaddstr((3*i)+7,18,pokes[i].name.c_str());	
		}
		attroff(COLOR_PAIR(11));
		attroff(COLOR_PAIR(12));
		mvaddstr(17,18,"PRESS <f> , <k/j> TO SELECT");
		mvaddch((3*selector)+7,17,'[');
		mvaddch((3*selector)+7 ,18+ pokes[selector].name.length(),']');
		commandShort = getch();
		if(commandShort == 'f'){
			globe.playerPokemon[0] = pokes[selector];
		}
		switch (commandShort)
		{
		case 'j':
			if (selector < 2)
				selector++;
			break;
			case 'k':
			if (selector > 0)
				selector--;
			break;
			case 'Q':
				return;
		}
	}
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
			oldMap[y][x] = (char*) tile_str[ROAD];
			continue;
		}
		if(y == 0){//a
			atod = true;
			y++;
			oldMap[y][x] = (char*) tile_str[ROAD];
			continue;;
		}
		if(y == MAPHEIGHT -1){//b
			//only b to d
			btod = true;
			y--;
			oldMap[y][x] = (char*) tile_str[ROAD];
			continue;;
		}	

		if(atod){
			if(targety > y){
				if(heightmap[y+1][x] < heightmap[y][x]+3){
					y++;;
				}else if(x == targetx){
					y++;
				}
				oldMap[y][x] = (char*) tile_str[ROAD];
			}
			if(targetx > x){
				x++;
				oldMap[y][x] = (char*) tile_str[ROAD];
			}
		}
		if(btod){
				if(targety < y){
				if(heightmap[y-1][x] < heightmap[y][x] + 3){
					y--;;
				}else if(x == targetx){
					y--;
				}
				oldMap[y][x] = (char*) tile_str[ROAD];
			}
			if(targetx > x){
				x++;
				oldMap[y][x] = (char*) tile_str[ROAD];
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
				oldMap[y][x] = (char*) tile_str[ROAD];
			}
			if(targetx > x){
				x++;
				oldMap[y][x] = (char*) tile_str[ROAD];
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
				oldMap[y][x] = (char*) tile_str[ROAD];
			}
			if(targetx > x){
				x++;
				oldMap[y][x] = (char*) tile_str[ROAD];
			}
		}
		counter ++;
		if(counter == 5000){
			x = targetx;
			y = targety;
			oldMap[8][35] = (char*)tile_str[ROAD];
			oldMap[12][45] = (char *)tile_str[ROAD];
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
			oldMap[0][a] = (char *)tile_str[ROAD];
			oldMap[c][0]= (char *)tile_str[ROAD];
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
			oldMap[MAPHEIGHT-1][b] = (char *)tile_str[ROAD];
			oldMap[c][0]= (char *)tile_str[ROAD];
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
			oldMap[0][a] = (char *)tile_str[ROAD];
			oldMap[MAPHEIGHT-1][b] = (char *)tile_str[ROAD];
			oldMap[c][0]= (char *)tile_str[ROAD];
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
			oldMap[0][a] = (char *)tile_str[ROAD];
			oldMap[d][MAPWIDTH -1] = (char *)tile_str[ROAD];
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
			oldMap[MAPHEIGHT-1][b] = (char *)tile_str[ROAD];
			oldMap[d][MAPWIDTH -1] = (char *)tile_str[ROAD];
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
			oldMap[0][a] = (char *)tile_str[ROAD];
			oldMap[MAPHEIGHT-1][b] = (char *)tile_str[ROAD];
			oldMap[d][MAPWIDTH -1] = (char *)tile_str[ROAD];
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
		oldMap[MAPHEIGHT-1][b] = (char *)tile_str[ROAD];
		oldMap[c][0]= (char *)tile_str[ROAD];
		oldMap[d][MAPWIDTH -1] = (char *)tile_str[ROAD];
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
		oldMap[0][a] = (char *)tile_str[ROAD];
		oldMap[c][0]= (char *)tile_str[ROAD];
		oldMap[d][MAPWIDTH -1] = (char *)tile_str[ROAD];
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
						oldMap[y][x-1] = (char *)tile_str[ROAD];
					}else if(heightmap[y][x+1] < heightmap[y][x]){
						x++;
					}else if(heightmap[y+1][x+1] < heightmap[y][x]){
						y++;
						x++;
						oldMap[y][x-1] = (char *)tile_str[ROAD];
					}else{
						y++;
					}
				}else if (x > targetx && x != targetx){//target to left
					if(abs(targetx-x) >= abs(targety-y)){
						if(y == 0){y++;}
						int ran = rand() % 10;
						if(ran > 7 && y != 0 && x < a - 1){y++;}
						x--;
						oldMap[y][x+1] = (char *)tile_str[ROAD];
					}else if(heightmap[y][x-1] < heightmap[y][x]){
						x--;
					}else if(heightmap[y+1][x-1] < heightmap[y][x]){
						y++;
						x--;
						oldMap[y][x+1] = (char *)tile_str[ROAD];
					}else{
						y++;
					}
				}else{
					y++;
				}			
				oldMap[y][x] = (char *)tile_str[ROAD];
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
					oldMap[y-1][x] = (char *)tile_str[ROAD];
				}else if(heightmap[y][x+1] < heightmap[y][x]){
					x++;
				}else if(heightmap[y+1][x+1] < heightmap[y][x] && targety != y){
					x++;
					y++;
					oldMap[y-1][x] = (char *)tile_str[ROAD];
					
				}else{
					x++;
				}
			}else if (y > targety && y != targety){
				if(abs(targety-y) >= abs(targetx-x)){
					if(x == 0){x++;}
					int ran = rand() % 10;
					if(ran > 7 && x != 0 && y < c + 1){x++;}
					y--;
					oldMap[y+1][x]= (char *)tile_str[ROAD];
				}else if(heightmap[y][x+1] < heightmap[y][x]){
					x++;
				}else if(heightmap[y-1][x+1] < heightmap[y][x] && targety != y){
					x++;
					y--;
					oldMap[y+1][x]= (char *)tile_str[ROAD];
				}else{
					x++;
				}

			}else{
				x++;
			}
			
				oldMap[y][x] = (char *)tile_str[ROAD];
			
		}else{
			cconnectd = true;
		}
		}

	oldMap[0][a] = (char *)tile_str[ROAD];
	oldMap[MAPHEIGHT-1][b] = (char *)tile_str[ROAD];
	oldMap[c][0]= (char *)tile_str[ROAD];
	oldMap[d][MAPWIDTH -1] = (char *)tile_str[ROAD];
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
		//std::cout<<"no mart"<<std::endl;
		mart = true;
	}
	chance = rand() % 100;
	if(chance > martchance){
		//std::cout<<"no center"<<std::endl;
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
					c = (char*) tile_str[MART];
					mart = true;
					martx = x;
					marty = y;
				}else{
					c = (char*) tile_str[CENTER];
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
void drawBox(int x,int y,int xl,int yl,int fill){
	if(fill)
		attron(COLOR_PAIR(16));
	    mvaddch(y,x,  ACS_ULCORNER); 
		yl = y + yl;
		xl = x + xl;
		for(int i = x+1; i <xl;i++){
			mvaddch(y,i,ACS_HLINE);
		}
		mvaddch(y,xl,  ACS_URCORNER); 
		for(int i = y+1; i < yl;i++){
			mvaddch(i,x,  ACS_VLINE);
			for(int j = x+1; j < xl;j++){
				if(fill)
					mvaddstr(i,j," ");
			} 
			mvaddch(i,xl, ACS_VLINE);
		}
		for(int i = x; i <xl;i++){
			mvaddch(yl,i,ACS_HLINE);
		}
		mvaddch(yl,x,ACS_LLCORNER);
		mvaddch(yl,xl,ACS_LRCORNER);
		attroff(COLOR_PAIR(16));
}
void printMap(map *Map){
	int i ,j;
	erase();
	addstr("\n");
	for(i = 0; i < MAPHEIGHT; i++){
		for(j= 0; j < MAPWIDTH; j++){
			if(Map->chars[i][j].symbol != NULL){
				if(!strcasecmp(character_str[PLAYER],Map->chars[i][j].symbol)){
						attron(COLOR_PAIR(18));
						attron(A_BLINK);
						addch(*Map->chars[i][j].symbol);
						attroff(COLOR_PAIR(18));
						attroff(A_BLINK);
				}else{
					attron(A_BLINK);
					attron(COLOR_PAIR(2));
					addch(*Map->chars[i][j].symbol);
					attroff(COLOR_PAIR(2));
					attroff(A_BLINK);
				}
			}else{
				if(!strcmp(tile_str[TREE],Map->tiles[i][j])){
					attron(COLOR_PAIR(10));
				}else if(!strcmp(tile_str[ROCK],Map->tiles[i][j])){
					attron(COLOR_PAIR(5));
				}else if(!strcmp(tile_str[ROAD],Map->tiles[i][j])){
					attron(COLOR_PAIR(15));
				}else if(!strcmp(tile_str[LONG],Map->tiles[i][j])){
					attron(COLOR_PAIR(13));
				}else if(!strcmp(tile_str[SHORT],Map->tiles[i][j])){
					attron(COLOR_PAIR(14));
				}else if(!strcmp(tile_str[WATER],Map->tiles[i][j])){
					attron(COLOR_PAIR(6));
				}else if(!strcmp(tile_str[MART],Map->tiles[i][j])){
					attron(COLOR_PAIR(8));
				}else if(!strcmp(tile_str[CENTER],Map->tiles[i][j])){
					attron(COLOR_PAIR(9));
				}
				addch(*Map->tiles[i][j]);
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
					attroff(COLOR_PAIR(13));
					attroff(COLOR_PAIR(14));
					attroff(COLOR_PAIR(15));
			}
			if(j == MAPWIDTH -1){
				addstr("\n");
			}
		}
	}
		if(fly){
		flyX = posx+200;
		flyY = posy+200;
		attron(COLOR_PAIR(1));
		fly = false;
		commandShort = ' ';
		bool validFly = false;
		char posY[12];
		int numy = posy-200;
		snprintf(posY,sizeof(posY),"%d",numy);
		mvaddstr(22,26,"Y:");
		mvaddstr(22,28,posY);
		char posX[12];
		int numx = posx -200;
		snprintf(posX,sizeof(posX),"%d",numx);
		mvaddstr(22,20,"X:");
		mvaddstr(22,22,posX);
		mvaddstr(22,32,"world pos");
		echo();
		curs_set(1);
		while(!validFly){
			mvaddstr(22,0,"fly to? ");
			mvaddstr(23,8," x y ");
			char format[] = " %d %d";
			mvscanw(22,8,format,&flyX,&flyY);

			if(flyX >= -200 && flyX <= 200
			&& flyY >= -200 && flyY <= 200){
				validFly = true;
				mvaddstr(23,0,"            ");
			}else{
				mvaddstr(22,0,"            ");
				mvaddstr(23,0,"Invalid fly!");
			}
		}
		globe.maps[posy][posx]->chars[globe.playerY][globe.playerX].symbol = NULL;
		noecho();
		curs_set(0);
		//movement here
		posx = flyX + 200;
		posy = flyY + 200;
		if(globe.maps[posy][posx] != NULL){
				bool genPlayer = false;
				int playerX,playerY;
			while(!genPlayer){
					playerX = rand() % 70; playerX+=5;  
				playerY = rand() % 15; playerY+=2;
				if(!strcmp(globe.maps[posy][posx]->tiles[playerY][playerX], tile_str[ROAD])){
					player.posX = playerX;
					player.posY = playerY;
					globe.playerX = playerX;
					globe.playerY = playerY;
					//globe.maps[posy][posx]->chars[playerY][playerX] = player;
					genPlayer = true;
					playerReturnToMapCalc(playerX,playerY);
				}
			}
		}else{
			globe.playerX = -9999;
			globe.playerY = -9999;
		}
	
		skipHeap = true;
		//mvaddstr(22,0,"                     ");
		loadMap();
		
	}
	attron(COLOR_PAIR(1));
	char posY[12];
	int numy = posy-200;
	snprintf(posY,sizeof(posY),"%d",numy);
	mvaddstr(22,6,"Y:");
	mvaddstr(22,8,posY);
	char posX[12];
	int numx = posx -200;
	snprintf(posX,sizeof(posX),"%d",numx);
	mvaddstr(22,0,"X:");
	mvaddstr(22,2,posX);
	mvaddstr(22,12,"world pos");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(3));
	char pposY[12];
	snprintf(pposY,sizeof(pposY),"%d",globe.playerY);
	mvaddstr(23,6,"Y:");
	mvaddstr(23,8,pposY);
	char pposX[12];
	snprintf(pposX,sizeof(pposX),"%d",globe.playerX);
	mvaddstr(23,0,"X:");
	mvaddstr(23,12,"player pos");
	mvaddstr(23,2,pposX);
	attroff(COLOR_PAIR(3));
	attron(COLOR_PAIR(6));
	mvaddstr(22,30,"command: ");
	mvaddch(22,38,commandShort);
	attroff(COLOR_PAIR(6));
	if(showingOverworldbag){
		attron(COLOR_PAIR(1));
		mvaddstr(0,0,"Pick pokemon to heal!!\n");
		attroff(COLOR_PAIR(1));
				//PRINTING FIRST BOX
		mvaddch(4,15,  ACS_ULCORNER); 
		for(int i = 16; i <64;i++){
			mvaddch(4,i,ACS_HLINE);
		}
		mvaddch(4,64,  ACS_URCORNER); 
		mvaddch(5,15,  ACS_VLINE); 
		mvaddstr(5,16, "                       Bag                       ");
		mvaddch(5,64,  ACS_VLINE); 
		mvaddch(6,15,  ACS_LTEE); 
		for(int i = 16; i <64;i++){
			mvaddch(6,i,ACS_HLINE);
		}
		mvaddch(6,64,  ACS_RTEE); 
		for(int i = 7; i < 18;i++){
			mvaddch(i,15,  ACS_VLINE); 
			mvaddstr(i,16,"                                                ");
			mvaddch(i,64, ACS_VLINE);
		}
		std::string p = std::to_string(potions) + "x Potions";
		mvaddstr(7,16,p.c_str());
		mvaddstr(17,16,"Press <f> to select, <k/j> up/down");
		std::string poke1,poke2,poke3,poke4,poke5,poke6;
		for(int i = 0; i < 6; i ++){
			if(globe.playerPokemon[i].name == "empty"){
				globe.playerPokemon[i].currHp = 0;
			}
		}
		poke1 = globe.playerPokemon[0].name + " HP:" + std::to_string(globe.playerPokemon[0].currHp);
		poke2 = globe.playerPokemon[1].name + " HP:" + std::to_string(globe.playerPokemon[1].currHp);
		poke3 = globe.playerPokemon[2].name + " HP:" + std::to_string(globe.playerPokemon[2].currHp);
		poke4 = globe.playerPokemon[3].name + " HP:" + std::to_string(globe.playerPokemon[3].currHp);
		poke5 = globe.playerPokemon[4].name + " HP:" + std::to_string(globe.playerPokemon[4].currHp);
		poke6 = globe.playerPokemon[5].name + " HP:" + std::to_string(globe.playerPokemon[5].currHp);
		mvaddstr(9,20,poke1.c_str());
		mvaddstr(10,20,poke2.c_str());
		mvaddstr(11,20,poke3.c_str());
		mvaddstr(12,20,poke4.c_str());
		mvaddstr(13,20,poke5.c_str());
		mvaddstr(14,20,poke6.c_str());
		switch(bagSelector){
			case 0:
			poke1 = "->" + poke1 + "<-";
			mvaddstr(9,18,poke1.c_str());
			break;
			case 1:
			poke2 = "->" + poke2 + "<-";
			mvaddstr(10,18,poke2.c_str());
			break;
			case 2:
			poke3 = "->" + poke3 + "<-";
			mvaddstr(11,18,poke3.c_str());
			break;
			case 3:
			poke4 = "->" + poke4 + "<-";
			mvaddstr(12,18,poke4.c_str());
			break;
			case 4:
			poke5 = "->" + poke5 + "<-";
			mvaddstr(13,18,poke5.c_str());
			break;
			case 5:
			poke6 = "->" + poke6 + "<-";
			mvaddstr(14,18,poke6.c_str());
			break;
		}


		for(int i = 16; i <64;i++){
			mvaddch(18,i,ACS_HLINE);
		}
				

		mvaddch(18,15,ACS_LLCORNER);
		mvaddch(18,64,ACS_LRCORNER);
	}
	if(inMart){
		attron(COLOR_PAIR(1));
		mvaddstr(0,0,"in pokemart!\n");
		attroff(COLOR_PAIR(1));
				//PRINTING FIRST BOX
		mvaddch(4,15,  ACS_ULCORNER); 
		for(int i = 16; i <64;i++){
			mvaddch(4,i,ACS_HLINE);
		}
		mvaddch(4,64,  ACS_URCORNER); 
		mvaddch(5,15,  ACS_VLINE); 
		mvaddstr(5,16, "                    PokeMart                     ");
		mvaddch(5,64,  ACS_VLINE); 
		mvaddch(6,15,  ACS_LTEE); 
		for(int i = 16; i <64;i++){
			mvaddch(6,i,ACS_HLINE);
		}
		mvaddch(6,64,  ACS_RTEE); 
		for(int i = 7; i < 18;i++){
			mvaddch(i,15,  ACS_VLINE); 
			mvaddstr(i,16,"                                                ");
			mvaddch(i,64, ACS_VLINE);
		}
		mvaddstr(8,16, "          all suppplies are restored!          ");
		for(int i = 16; i <64;i++){
			mvaddch(18,i,ACS_HLINE);
		}
				

		mvaddch(18,15,ACS_LLCORNER);
		mvaddch(18,64,ACS_LRCORNER);
	}
	if(inCenter){
		attron(COLOR_PAIR(1));
		mvaddstr(0,0,"in pokecenter!\n");
		attroff(COLOR_PAIR(1));
		
		//PRINTING FIRST BOX
		mvaddch(4,15,  ACS_ULCORNER); 
		for(int i = 16; i <64;i++){
			mvaddch(4,i,ACS_HLINE);
		}
		mvaddch(4,64,  ACS_URCORNER); 
		mvaddch(5,15,  ACS_VLINE); 
		mvaddstr(5,16, "                   PokeCenter                    ");
		
		mvaddch(5,64,  ACS_VLINE); 
		mvaddch(6,15,  ACS_LTEE); 
		for(int i = 16; i <64;i++){
			mvaddch(6,i,ACS_HLINE);
		}
		mvaddch(6,64,  ACS_RTEE); 
		for(int i = 7; i < 18;i++){
			mvaddch(i,15,  ACS_VLINE); 
			mvaddstr(i,16,"                                                ");
			mvaddch(i,64, ACS_VLINE);
		}
		
		mvaddstr(8,16, "            all pokemon are healed!            ");
		for(int i = 16; i <64;i++){
			mvaddch(18,i,ACS_HLINE);
		}
		mvaddch(18,15,ACS_LLCORNER);
		mvaddch(18,64,ACS_LRCORNER);
	}
	if(inBattle){
		attron(COLOR_PAIR(1));
		mvaddstr(23,30,"PRESS F TO SELECT");
		mvaddstr(0,0,"in battle with");
		attron(COLOR_PAIR(8));
		mvaddstr(0,15,opponent->symbol);
		attron(COLOR_PAIR(1));
		mvaddstr(0,16,"!!");
		attroff(COLOR_PAIR(1));
		attroff(COLOR_PAIR(8));
		
		pokemonObject poke = *opponent->currentPoke;
		pokemonObject *playerPoke = globe.playerCurrentPokemon;

		drawBox(0,1,79,20,0);//whole box
		drawBox(2,2,37,4,1);//trainer box	
			attron(COLOR_PAIR(17));
			mvaddstr(3,4,poke.name.c_str());
			attroff(COLOR_PAIR(17));
			attron(COLOR_PAIR(16));
			mvaddstr(3,5 + poke.name.length(),"lvl.");
			mvaddstr(3,9 + poke.name.length(),std::to_string(poke.level).c_str());
			if(poke.shiny){
				mvaddstr(3,9 + poke.name.length() + 4 ,"*");
			}
			mvaddstr(4,8,std::to_string(opponent->currentPoke->currHp).c_str());
			mvaddstr(4,4,"HP:");
			//mvaddstr(4,10,std::to_string(debugInt).c_str());
			mvaddstr(5,4,"[");mvaddstr(5,37,"]"); //BAR SIZE IS 32
			double ratio = (poke.currHp)/(double)poke.hp; //gets percentage
			ratio = ratio *32;
			ratio = std::ceil(ratio);
			for(int i = 0;i < ratio;i++){
				attron(COLOR_PAIR(17));
				mvaddstr(5,i+5,"#");
				attroff(COLOR_PAIR(17));
			}
		drawBox(40,8,37,4,1);//player box
			attron(COLOR_PAIR(17));
			mvaddstr(9,42,playerPoke->name.c_str());
			attroff(COLOR_PAIR(17));
			attron(COLOR_PAIR(16));
			mvaddstr(9,43 + playerPoke->name.length(),"lvl.");
			mvaddstr(9,47 + playerPoke->name.length(),std::to_string(playerPoke->level).c_str());
			if(playerPoke->shiny){
				mvaddstr(9,47 + playerPoke->name.length() + 4 ,"*");
			}
			mvaddstr(10,46,std::to_string(playerPoke->currHp).c_str());
			mvaddstr(10,42,"HP:");
			mvaddstr(11,42,"[");mvaddstr(11,75,"]"); //BAR SIZE IS 32
			ratio = (playerPoke->currHp)/(double)playerPoke->hp; //gets percentage
			ratio = ratio *32;
			ratio = std::ceil(ratio);
			for(int i = 0;i < ratio;i++){
				attron(COLOR_PAIR(17));
				mvaddstr(11,i+43,"#");
				attroff(COLOR_PAIR(17));
			}
		drawBox(2,14,75,6,1);//action box
			if(playerBattleTurn){
				if(battleMessage != "empty"){
					attron(COLOR_PAIR(16));
					mvaddstr(17,4,battleMessage.c_str());
					refresh();
					usleep(1000000);
					battleMessage = "empty";
					printMap(currentMap);
				}else{
					attron(COLOR_PAIR(16));
					mvaddstr(17,4,"What will ");
					attron(COLOR_PAIR(17));
					mvaddstr(17,14,playerPoke->name.c_str());
					attron(COLOR_PAIR(16));
					mvaddstr(17,15 +playerPoke->name.length(),"do?");
				}
				
				attron(COLOR_PAIR(16));
				if(showingMoves){
					std::string move1,move2,move3,move4;
					move1 = playerPoke->availableMoves[0].indentifier;
					move2 = playerPoke->availableMoves[1].indentifier;
					move3 = playerPoke->availableMoves[2].indentifier;
					move4 = playerPoke->availableMoves[3].indentifier;
					mvaddstr(16,40,move1.c_str());
					mvaddstr(16,60,move2.c_str());
					mvaddstr(18,40,move3.c_str());
					mvaddstr(18,60,move4.c_str());
					switch (battleSelector)
					{
						case 0:
						move1 = "->"+playerPoke->availableMoves[0].indentifier+"<-";
						mvaddstr(16,38,move1.c_str());
							break;
						case 1:
						move2 = "->"+playerPoke->availableMoves[1].indentifier+"<-";
						mvaddstr(16,58,move2.c_str());
							break;
						case 2:
						move3 = "->"+playerPoke->availableMoves[2].indentifier+"<-";
						mvaddstr(18,38,move3.c_str());
							break;
						case 3:
						move4 = "->"+playerPoke->availableMoves[3].indentifier+"<-";
						mvaddstr(18,58,move4.c_str());
							break;
					}
				}else if(showingBag){
					std::string r,p,pb;
					r = std::to_string(revives) + "x revives";
					p = std::to_string(potions) + "x potions";
					pb = std::to_string(pokeballs) + "x pokeballs";
					mvaddstr(16,40,r.c_str());
					mvaddstr(17,40,p.c_str());
					mvaddstr(18,40,pb.c_str());
					switch (battleSelector)
					{
						case 0:
						r= "->" + r + "<-";
						mvaddstr(16,38,r.c_str());
							break;
						case 1:
						p= "->" + p + "<-";
						mvaddstr(17,38,p.c_str());
							break;
						case 2:
						pb= "->" + pb + "<-";
						mvaddstr(18,38,pb.c_str());
							break;
					}
				}else if(showingSwitchPoke){
					std::string poke1,poke2,poke3,poke4,poke5,poke6;
					poke1 = globe.playerPokemon[0].name.c_str();
					poke2 = globe.playerPokemon[1].name.c_str();
					poke3 = globe.playerPokemon[2].name.c_str();
					poke4 = globe.playerPokemon[3].name.c_str();
					poke5 = globe.playerPokemon[4].name.c_str();
					poke6 = globe.playerPokemon[5].name.c_str();
					mvaddstr(16,35,globe.playerPokemon[0].name.c_str());
					mvaddstr(16,55,globe.playerPokemon[1].name.c_str());
					mvaddstr(17,35,globe.playerPokemon[2].name.c_str());
					mvaddstr(17,55,globe.playerPokemon[3].name.c_str());
					mvaddstr(18,35,globe.playerPokemon[4].name.c_str());
					mvaddstr(18,55,globe.playerPokemon[5].name.c_str());	
					switch (battleSelector)
					{
						case 0:
						poke1= "->" + poke1 + "<-";
						mvaddstr(16,33,poke1.c_str());
							break;
						case 1:
						poke2= "->" + poke2 + "<-";
						mvaddstr(16,53,poke2.c_str());
							break;
						case 2:
						poke3= "->" + poke3 + "<-";
						mvaddstr(17,33,poke3.c_str());
							break;
						case 3:
						poke4= "->" + poke4 + "<-";
						mvaddstr(17,53,poke4.c_str());
							break;
						case 4:
						poke5= "->" + poke5 + "<-";
						mvaddstr(18,33,poke5.c_str());
							break;	
						case 5:
						poke6= "->" + poke6 + "<-";
						mvaddstr(18,53,poke6.c_str());
							break;
					}
					
					
				}else{
					attron(COLOR_PAIR(16));
					mvaddstr(16,40,"[FIGHT]");
					mvaddstr(16,60,"[BAG]");
					mvaddstr(18,40,"[RUN]");
					mvaddstr(18,60,"[POKEMON]");
					switch (battleSelector)
					{
						case 0:
						mvaddstr(16,38,"->[FIGHT]<-");
							break;
						case 1:
						mvaddstr(16,58,"->[BAG]<-");
							break;
						case 2:
						mvaddstr(18,38,"->[RUN]<-");
							break;
						case 3:
						mvaddstr(18,58,"->[POKEMON]<-");
							break;
					}
				}
				
			}else{
				//other trainer did something text
			}
		attroff(COLOR_PAIR(16));
	}
	//80/21
	if(showingList){

		//PRINTING FIRST BOX
		mvaddch(4,15,  ACS_ULCORNER); 
		for(int i = 16; i <64;i++){
			mvaddch(4,i,ACS_HLINE);
		}
		mvaddch(4,64,  ACS_URCORNER); 
		mvaddch(5,15,  ACS_VLINE); 
		mvaddstr(5,16, "          TRAINERS DISTANCE TO PLAYER            ");
		mvaddch(5,64,  ACS_VLINE); 
		mvaddch(6,15,  ACS_LTEE); 
		for(int i = 16; i <64;i++){
			mvaddch(6,i,ACS_HLINE);
		}
		mvaddch(6,64,  ACS_RTEE); 
		for(int i = 7; i < 18;i++){
			mvaddch(i,15,  ACS_VLINE); 
			mvaddstr(i,16,"                                                ");
			mvaddch(i,64, ACS_VLINE);
		}
		for(int i = 16; i <64;i++){
			mvaddch(18,i,ACS_HLINE);
		}
		mvaddch(18,15,ACS_LLCORNER);
		mvaddch(18,64,ACS_LRCORNER);
		//PRINTING REST OF THING.
		int numOfChars = 0;
		character_c chars[numTrainers];
		for(int i = 1; i < MAPHEIGHT - 1;i++){
			for(int j = 1; j < MAPWIDTH - 1;j++){
				if(currentMap->chars[i][j].symbol != NULL &&
				currentMap->chars[i][j].symbol != character_str[PLAYER]){
					chars[numOfChars] = currentMap->chars[i][j];
					numOfChars++;
				}
			}
		}
		int j = 0;
		int num = 10;
		if(numTrainers < 10){
			num = numTrainers;
		}
		for(int i = trainerListOffset; i < trainerListOffset + num;i++){
			if(chars[i].defeated){
				mvaddstr(7+j,1+UIMIDOFFSET,"defeated");
			}
			char nextTurn[15];    
			snprintf(nextTurn,15,"%d",chars[i].nextTurn );   
			mvaddstr(7+j,40 + UIMIDOFFSET,nextTurn);
			mvaddstr(7+j,36 + UIMIDOFFSET,"nt: ");
			attron(COLOR_PAIR(2));
			if(trainerListOffset > 0)
				mvaddch(7,35 +UIMIDOFFSET,'^');
			if(trainerListOffset < numTrainers - 10)
				mvaddch(17,35 +UIMIDOFFSET,'v');
			char index[15];
			snprintf(index,15,"%d",j + trainerListOffset + 1);
			mvaddstr(7+j,10 + UIMIDOFFSET,index);
			mvaddch(7+j,15 + UIMIDOFFSET, *chars[i].symbol);
			attroff(COLOR_PAIR(2));
			mvaddstr(7+j,17+ UIMIDOFFSET,"->");
			char trainerX[4];
			char trainerY[4];
			snprintf(trainerX,4,"%d",chars[i].posX - globe.playerX);
			snprintf(trainerY,4,"%d",chars[i].posY - globe.playerY);
			mvaddstr(7+j,20 +UIMIDOFFSET,"X:");
			mvaddstr(7+j,23 +UIMIDOFFSET,trainerX);
			mvaddstr(7+j,27 +UIMIDOFFSET,"Y:");
			mvaddstr(7+j,30 +UIMIDOFFSET,trainerY);
			j++;
		}
		char str[10];
		snprintf(str,10,"%d",numOfChars);
    	mvaddstr(17,16,"num of trainers: ");mvaddstr(17,33,str);
		
		
	}

	if(inEcounter){
				attron(COLOR_PAIR(1));
				mvaddstr(23,30,"PRESS F TO SELECT");
		mvaddstr(0,0,"in battle with a wild");
		attron(COLOR_PAIR(8));
		mvaddstr(0,22,encounteredPoke.name.c_str());
		attron(COLOR_PAIR(1));
		mvaddstr(0,22+encounteredPoke.name.length(),"!!");
		attroff(COLOR_PAIR(1));
		attroff(COLOR_PAIR(8));
		
		pokemonObject poke = encounteredPoke;
		pokemonObject *playerPoke = globe.playerPokemon;

		drawBox(0,1,79,20,0);//whole box
		drawBox(2,2,37,4,1);//trainer box	
			attron(COLOR_PAIR(17));
			mvaddstr(3,4,poke.name.c_str());
			attroff(COLOR_PAIR(17));
			attron(COLOR_PAIR(16));
			mvaddstr(3,5 + poke.name.length(),"lvl.");
			mvaddstr(3,9 + poke.name.length(),std::to_string(poke.level).c_str());
			if(poke.shiny){
				mvaddstr(3,9 + poke.name.length() + 4 ,"*");
			}
			mvaddstr(4,8,std::to_string(poke.currHp).c_str());
			mvaddstr(4,4,"HP:");
			//mvaddstr(4,10,std::to_string(debugInt).c_str());
			mvaddstr(5,4,"[");mvaddstr(5,37,"]"); //BAR SIZE IS 32
			double ratio = (poke.currHp)/(double)poke.hp; //gets percentage
			ratio = ratio *32;
			ratio = std::ceil(ratio);
			for(int i = 0;i < ratio;i++){
				attron(COLOR_PAIR(17));
				mvaddstr(5,i+5,"#");
				attroff(COLOR_PAIR(17));
			}
		drawBox(40,8,37,4,1);//player box
			attron(COLOR_PAIR(17));
			mvaddstr(9,42,playerPoke->name.c_str());
			attroff(COLOR_PAIR(17));
			attron(COLOR_PAIR(16));
			mvaddstr(9,43 + playerPoke->name.length(),"lvl.");
			mvaddstr(9,47 + playerPoke->name.length(),std::to_string(playerPoke->level).c_str());
			if(playerPoke->shiny){
				mvaddstr(9,47 + playerPoke->name.length() + 4 ,"*");
			}
			mvaddstr(10,46,std::to_string(playerPoke->currHp).c_str());
			mvaddstr(10,42,"HP:");
			mvaddstr(11,42,"[");mvaddstr(11,75,"]"); //BAR SIZE IS 32
			ratio = (playerPoke->currHp)/(double)playerPoke->hp; //gets percentage
			ratio = ratio *32;
			ratio = std::ceil(ratio);
			for(int i = 0;i < ratio;i++){
				attron(COLOR_PAIR(17));
				mvaddstr(11,i+43,"#");
				attroff(COLOR_PAIR(17));
			}
		drawBox(2,14,75,6,1);//action box
			if(playerBattleTurn){
				if(battleMessage != "empty"){
					attron(COLOR_PAIR(16));
					mvaddstr(17,4,battleMessage.c_str());
					refresh();
					usleep(1000000);
					battleMessage = "empty";
					printMap(currentMap);
				}else{
					attron(COLOR_PAIR(16));
					mvaddstr(17,4,"What will ");
					attron(COLOR_PAIR(17));
					mvaddstr(17,14,playerPoke->name.c_str());
					attron(COLOR_PAIR(16));
					mvaddstr(17,15 +playerPoke->name.length(),"do?");
				}
				
				attron(COLOR_PAIR(16));
				if(showingMoves){
					std::string move1,move2,move3,move4;
					move1 = playerPoke->availableMoves[0].indentifier;
					move2 = playerPoke->availableMoves[1].indentifier;
					move3 = playerPoke->availableMoves[2].indentifier;
					move4 = playerPoke->availableMoves[3].indentifier;
					mvaddstr(16,40,move1.c_str());
					mvaddstr(16,60,move2.c_str());
					mvaddstr(18,40,move3.c_str());
					mvaddstr(18,60,move4.c_str());
					switch (battleSelector)
					{
						case 0:
						move1 = "->"+playerPoke->availableMoves[0].indentifier+"<-";
						mvaddstr(16,38,move1.c_str());
							break;
						case 1:
						move2 = "->"+playerPoke->availableMoves[1].indentifier+"<-";
						mvaddstr(16,58,move2.c_str());
							break;
						case 2:
						move3 = "->"+playerPoke->availableMoves[2].indentifier+"<-";
						mvaddstr(18,38,move3.c_str());
							break;
						case 3:
						move4 = "->"+playerPoke->availableMoves[3].indentifier+"<-";
						mvaddstr(18,58,move4.c_str());
							break;
					}
				}else if(showingBag){
					std::string r,p,pb;
					r = std::to_string(revives) + "x revives";
					p = std::to_string(potions) + "x potions";
					pb = std::to_string(pokeballs) + "x pokeballs";
					mvaddstr(16,40,r.c_str());
					mvaddstr(17,40,p.c_str());
					mvaddstr(18,40,pb.c_str());
					switch (battleSelector)
					{
						case 0:
						r= "->" + r + "<-";
						mvaddstr(16,38,r.c_str());
							break;
						case 1:
						p= "->" + p + "<-";
						mvaddstr(17,38,p.c_str());
							break;
						case 2:
						pb= "->" + pb + "<-";
						mvaddstr(18,38,pb.c_str());
							break;
					}
				}else if(showingSwitchPoke){
					std::string poke1,poke2,poke3,poke4,poke5,poke6;
					poke1 = globe.playerPokemon[0].name.c_str();
					poke2 = globe.playerPokemon[1].name.c_str();
					poke3 = globe.playerPokemon[2].name.c_str();
					poke4 = globe.playerPokemon[3].name.c_str();
					poke5 = globe.playerPokemon[4].name.c_str();
					poke6 = globe.playerPokemon[5].name.c_str();
					mvaddstr(16,35,globe.playerPokemon[0].name.c_str());
					mvaddstr(16,55,globe.playerPokemon[1].name.c_str());
					mvaddstr(17,35,globe.playerPokemon[2].name.c_str());
					mvaddstr(17,55,globe.playerPokemon[3].name.c_str());
					mvaddstr(18,35,globe.playerPokemon[4].name.c_str());
					mvaddstr(18,55,globe.playerPokemon[5].name.c_str());	
					switch (battleSelector)
					{
						case 0:
						poke1= "->" + poke1 + "<-";
						mvaddstr(16,33,poke1.c_str());
							break;
						case 1:
						poke2= "->" + poke2 + "<-";
						mvaddstr(16,53,poke2.c_str());
							break;
						case 2:
						poke3= "->" + poke3 + "<-";
						mvaddstr(17,33,poke3.c_str());
							break;
						case 3:
						poke4= "->" + poke4 + "<-";
						mvaddstr(17,53,poke4.c_str());
							break;
						case 4:
						poke5= "->" + poke5 + "<-";
						mvaddstr(18,33,poke5.c_str());
							break;	
						case 5:
						poke6= "->" + poke6 + "<-";
						mvaddstr(18,53,poke6.c_str());
							break;
					}
					
					
				}else{
					attron(COLOR_PAIR(16));
					mvaddstr(16,40,"[FIGHT]");
					mvaddstr(16,60,"[BAG]");
					mvaddstr(18,40,"[RUN]");
					mvaddstr(18,60,"[POKEMON]");
					switch (battleSelector)
					{
						case 0:
						mvaddstr(16,38,"->[FIGHT]<-");
							break;
						case 1:
						mvaddstr(16,58,"->[BAG]<-");
							break;
						case 2:
						mvaddstr(18,38,"->[RUN]<-");
							break;
						case 3:
						mvaddstr(18,58,"->[POKEMON]<-");
							break;
					}
				}
				
			}else{
				//other trainer did something text
			}
		attroff(COLOR_PAIR(16));
	}
	if(showingPoke){
		
		mvaddch(4,15,  ACS_ULCORNER); 
		for(int i = 16; i <64;i++){
			mvaddch(4,i,ACS_HLINE);
		}
		mvaddch(4,64,  ACS_URCORNER); 
		mvaddch(5,15,  ACS_VLINE); 
		mvaddstr(5,16, "                PLAYER POKEMON                   ");
		mvaddch(5,64,  ACS_VLINE); 
		mvaddch(6,15,  ACS_LTEE); 
		for(int i = 16; i <64;i++){
			mvaddch(6,i,ACS_HLINE);
		}
		mvaddch(6,64,  ACS_RTEE); 
		for(int i = 7; i < 19;i++){
			mvaddch(i,15,  ACS_VLINE); 
			mvaddstr(i,16,"                                                ");
			mvaddch(i,64, ACS_VLINE);
		}
		for(int i = 16; i <64;i++){
			mvaddch(19,i,ACS_HLINE);
		}
		mvaddch(19,15,ACS_LLCORNER);
		mvaddch(19,64,ACS_LRCORNER);
		int offset = 0;
		for(int i = 0; i < 6;i++){
			if(globe.playerPokemon[i].name == "empty"){
				continue;
			}
			attron(COLOR_PAIR(11));
			mvaddstr((2*i)+7,23+ globe.playerPokemon[i].name.length(), "id:");
			attron(COLOR_PAIR(12));
			mvaddstr((2*i)+7,26+ globe.playerPokemon[i].name.length(), std::to_string(globe.playerPokemon[i].id).c_str());
			attron(COLOR_PAIR(12));
			mvaddstr((2*i)+7,22,globe.playerPokemon[i].name.c_str());
			attron(COLOR_PAIR(11));
			mvaddstr((2*i)+7,27+ globe.playerPokemon[i].name.length() + std::to_string(globe.playerPokemon[i].id).length()
					,"lvl:");
			attron(COLOR_PAIR(12));
			mvaddstr((2*i)+7,31+ globe.playerPokemon[i].name.length() + std::to_string(globe.playerPokemon[i].id).length()
					,std::to_string(globe.playerPokemon[i].level).c_str());
			attron(COLOR_PAIR(11));
			mvaddstr((2*i)+7,32+ std::to_string(globe.playerPokemon[i].level).length() + globe.playerPokemon[i].name.length() + std::to_string(globe.playerPokemon[i].id).length()
					,"G:");
			attron(COLOR_PAIR(12));
			mvaddstr((2*i)+7,35 + globe.playerPokemon[i].name.length() + std::to_string(globe.playerPokemon[i].id).length()
					,std::to_string(globe.playerPokemon[i].gender).c_str());
			attron(COLOR_PAIR(11));
			mvaddstr((2*i)+7,37+ globe.playerPokemon[i].name.length() + std::to_string(globe.playerPokemon[i].id).length()
					,"S:");
			attron(COLOR_PAIR(12));
			mvaddstr((2*i)+7,39+ globe.playerPokemon[i].name.length() + std::to_string(globe.playerPokemon[i].id).length()
					,std::to_string(globe.playerPokemon[i].shiny).c_str());
			attron(COLOR_PAIR(11));
			mvaddstr((2*i)+8,16,"HP:");
			attron(COLOR_PAIR(12));
			mvaddstr((2*i)+8,19,std::to_string(globe.playerPokemon[i].currHp).c_str());
			offset = 20 + std::to_string(globe.playerPokemon[i].currHp).length();
			attron(COLOR_PAIR(11));
			mvaddstr((2*i)+8,offset,"ATK:");
			attron(COLOR_PAIR(12));
			mvaddstr((2*i)+8,offset+=4,std::to_string(globe.playerPokemon[i].curratk).c_str());
			offset +=std::to_string(globe.playerPokemon[i].curratk).length() + 1;
			attron(COLOR_PAIR(11));
			mvaddstr((2*i)+8,offset,"DEF:");
			attron(COLOR_PAIR(12));
			mvaddstr((2*i)+8,offset+=4,std::to_string(globe.playerPokemon[i].currdef).c_str());
			offset += 1 +std::to_string(globe.playerPokemon[i].currdef).length();
			attron(COLOR_PAIR(11));
			mvaddstr((2*i)+8,offset,"SPD:");
			attron(COLOR_PAIR(12));
			mvaddstr((2*i)+8,offset+=4,std::to_string(globe.playerPokemon[i].currspd).c_str());
			offset += 1 + std::to_string(globe.playerPokemon[i].currspd).length();
			attron(COLOR_PAIR(11));
			mvaddstr((2*i)+8,offset,"sATK:");
			attron(COLOR_PAIR(12));
			mvaddstr((2*i)+8,offset+=5,std::to_string(globe.playerPokemon[i].currsatk).c_str());
			offset += 1 + std::to_string(globe.playerPokemon[i].currsatk).length();
			attron(COLOR_PAIR(11));
			mvaddstr((2*i)+8,offset,"sDEF:");
			attron(COLOR_PAIR(12));
			mvaddstr((2*i)+8,offset+=5,std::to_string(globe.playerPokemon[i].currsdef).c_str());
		}
		attroff(COLOR_PAIR(11));
		attroff(COLOR_PAIR(12));

	}
	
	refresh();
}
map* createMap() {
    map* newMap = new map;
    newMap->generated = false;
	heap_init(&newMap->localHeap,char_cmp,NULL);
    for (int i = 0; i < MAPHEIGHT; ++i) {
        for (int j = 0; j < MAPWIDTH; ++j) {
            newMap->tiles[i][j] = NULL;
			newMap->chars[i][j].symbol = NULL;
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
	destination->localHeap = source->localHeap;
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

	for (y =0; y < MAPHEIGHT ; y++) {
	    for (x = 0; x < MAPWIDTH ; x++) {
    	  	path[y][x].hn = heap_insert(&h, &path[y][x]);
			}
	}

	 while ((p = static_cast<path_t*>(heap_remove_min(&h)))) {
  		p->hn = NULL;
		//printf("P COST : %d y: %d x: %d\n",p->cost,p->pos[1], p->pos[0]);
		int alt;

		if(p->pos[1] >= 1 && p->pos[1] <= 19 && p->pos[0] >= 1 && p->pos[0] <= 78){
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
void playerReturnToMapCalc(int playerX, int playerY){
	int min = INT32_MAX;
	if(globe.maps[posy][posx] != NULL){
		for(int i = 1; i < MAPHEIGHT -1;i++){
			for(int j = 1; j < MAPWIDTH - 1;j++){
				if(globe.maps[posy][posx]->chars[i][j].symbol != NULL &&
				strcmp(globe.maps[posy][posx]->chars[i][j].symbol,character_str[PLAYER])&&
				strcmp(globe.maps[posy][posx]->chars[i][j].symbol,character_str[SENTRIES])&&
				!globe.maps[posy][posx]->chars[i][j].defeated){
						if(globe.maps[posy][posx]->chars[i][j].nextTurn < min &&
						globe.maps[posy][posx]->chars[i][j].nextTurn > 0){
							min = globe.maps[posy][posx]->chars[i][j].nextTurn;
						}
					}
				}
		}
		if(min == INT32_MAX){
			min = 0;
		}
	player.nextTurn = min;
	if(globe.maps[posy][posx]->chars[playerY][playerX].symbol != NULL){
		//check surrounding tiles around enterance
		for(int i = 0; i < 8;i++){
			int testX = playerX + dirX[i];
			int testY = playerY + dirY[i];
			if(testX > 0 && testX < MAPWIDTH-1 
			&& testY > 0 && testY < MAPHEIGHT-1
			&& globe.maps[posy][posx]->chars[testY][testX].symbol == NULL
			&& getTileCost(globe.maps[posy][posx]->tiles[testY][testX],99) != INT16_MAX){
				playerX = testX;
				playerY = testY;
				globe.playerX = playerX;
				globe.playerY = playerY;
				player.posX = playerX;
				player.posY = playerY;
				break;
			}
		}
	}
	globe.maps[posy][posx]->chars[playerY][playerX] = player;
	globe.maps[posy][posx]->chars[playerY][playerX].hn = 
	heap_insert(&globe.maps[posy][posx]->localHeap, &globe.maps[posy][posx]->chars[playerY][playerX]);
						
	}
	skipHeap = true;
	loadMap();
}
void setNextPace(int nextY,int nextX,int posY, int posX,int dir,int type){
	int nextx = nextX;
	int nexty = nextY;
	char *currentTile = currentMap->chars[posY][posX].tile;
	if(dir == 0){//left
		if(currentMap->rivalMap[nextY][nextX - 1] != INT16_MAX
		&& currentMap->chars[nextY][nextX - 1].symbol == NULL
		&& posX > 1){
			nextx = nextX-1;
		}
		else if(currentMap->rivalMap[nextY][nextX - 1] != INT16_MAX
		&& currentMap->chars[nextY][nextX - 1].symbol == character_str[PLAYER]
		&& posX > 1){
			opponent = &currentMap->chars[posY][posX];
			inBattle = true;
				while (inBattle)
				{
					handleBattle(currentMap->chars[posY][posX],0);
				}
				nextx = posX;
				nexty = posY;
		}
		else{
			if(type == 3 || type == 4)
				dir = rand()%4;
			else
				dir = 1;
		}
	}else if(dir == 1){//right
		if(currentMap->rivalMap[nextY][nextX + 1] != INT16_MAX
		&& currentMap->chars[nextY][nextX + 1].symbol == NULL
		&& posX < MAPWIDTH - 2){
			nextx = nextX+1;
		}
		else if(currentMap->rivalMap[nextY][nextX + 1] != INT16_MAX
		&& currentMap->chars[nextY][nextX + 1].symbol == character_str[PLAYER]
		&& posX > 1){
			opponent = &currentMap->chars[posY][posX];
			inBattle = true;
				while (inBattle)
				{
					handleBattle(currentMap->chars[posY][posX],0);
				}
				nextx = posX;
				nexty = posY;
		}
		else{
			if(type == 3 || type == 4)
				dir = rand()%4;
			else
				dir = 0;
		}
	}else if(dir == 2){//up
		if(currentMap->rivalMap[nextY-1][nextX] != INT16_MAX
		&& currentMap->chars[nextY-1][nextX].symbol == NULL
		&& posY > 2){
			nexty = nextY-1;
		}
		else if(currentMap->rivalMap[nextY-1][nextX] != INT16_MAX
		&& currentMap->chars[nextY-1][nextX].symbol == character_str[PLAYER]
		&& posX > 1){
			opponent = &currentMap->chars[posY][posX];
			inBattle = true;
				while (inBattle)
				{
					handleBattle(currentMap->chars[posY][posX],0);
				}
				nextx = posX;
				nexty = posY;
		}
		else{
			if(type == 3 || type == 4)
				dir = rand()%4;
			else
				dir = 3;
		}
	}else if(dir == 3){//down
		if(currentMap->rivalMap[nextY+1][nextX] != INT16_MAX
		&& currentMap->chars[nextY+1][nextX].symbol == NULL
		&& posY < MAPHEIGHT - 2){
			nexty = nextY+1;
		}
		else if(currentMap->rivalMap[nextY+1][nextX] != INT16_MAX
		&& currentMap->chars[nextY+1][nextX].symbol == character_str[PLAYER]
		&& posX > 1){
			opponent = &currentMap->chars[posY][posX];
			inBattle = true;
				while (inBattle)
				{
					handleBattle(currentMap->chars[posY][posX],0);
				}
				nextx = posX;
				nexty = posY;
		}
		else{
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
			if(posY +dirY[i] > 19 || posY+dirY[i] < 1 
			|| posX +dirX[i] > 79 || posX+dirX[i] < 1){
				continue;
			}
		if(type == 0){

				//check surrounding for lowest cost
			if(currentMap->hikerMap[posY+dirY[i]][posX+dirX[i]] < min  && 
			(currentMap->chars[posY+dirY[i]][posX+dirX[i]].symbol == NULL || currentMap->chars[posY+dirY[i]][posX+dirX[i]].symbol == character_str[PLAYER])){
				nextx = posX + dirX[i];
				nexty = posY + dirY[i];
				min = currentMap->hikerMap[nexty][nextx];
			}
		}else if(type==1){
			if(currentMap->rivalMap[posY+dirY[i]][posX+dirX[i]] < min  && 
			(currentMap->chars[posY+dirY[i]][posX+dirX[i]].symbol == NULL || currentMap->chars[posY+dirY[i]][posX+dirX[i]].symbol == character_str[PLAYER])){				nextx = posX + dirX[i];
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
	while((c = static_cast<character_c*>(heap_remove_min(&currentMap->localHeap)))){
		 if(c != NULL){
			if(c->nextTurn > currentTime){
				currentTime = currentMap->chars[c->posY][c->posX].nextTurn;
				heap_insert(&currentMap->localHeap, &currentMap->chars[c->posY][c->posX]);
				break;
			}
			if(c->symbol == NULL)
				continue;;
			if(!strcmp(c->symbol,character_str[PLAYER])){
				//int num = rand() % 4;
				playerTurn = true;
				int playerX = player.posX;
				int playerY = player.posY;
				bool validCommand = false;
				while(!validCommand){	
					printMap(currentMap);
					commandShort = ' ';
					commandShort = getch();
					validCommand = true;
					if(showingList){
						switch(commandShort){
							case 't':
								showingList = !showingList;
								validCommand = false;
								break;
							case KEY_UP:
								if(trainerListOffset > 0)
									trainerListOffset--;
								validCommand = false;
								break;
							case KEY_DOWN:
								if(trainerListOffset < numTrainers - 10)
									trainerListOffset++;
								validCommand = false;
								break;
							case KEY_ESC:
								showingList = false;
								break;
							case 'Q':
								break;
							default:
								validCommand = false;
								break;
						}
					}else if(showingPoke){
						switch(commandShort){
							case 'p':
								showingPoke = !showingPoke;
								validCommand = false;
								break;
							case KEY_ESC:
								showingPoke = false;
								break;
							case 'Q':
								break;
							default:
								validCommand = false;
								break;
						}
					}else if(showingOverworldbag){
						printMap(currentMap);
						switch(commandShort){
							case 'B':
								showingOverworldbag = !showingOverworldbag;
								validCommand = false;
								break;
							case KEY_ESC:
								showingOverworldbag = false;
								break;
							case 'f':
								validCommand = false;
								switch(bagSelector){
									case 0:
										if(globe.playerPokemon[0].name != "empty"
										&& globe.playerPokemon[0].currHp < globe.playerPokemon[0].hp){
											globe.playerPokemon[0].currHp += 20;
											if(globe.playerPokemon[0].currHp > globe.playerPokemon[0].hp){
												globe.playerPokemon[0].currHp = globe.playerPokemon[0].hp;
											}
											potions--;
										}
									break;
									case 1:
										if(globe.playerPokemon[1].name != "empty"
										&& globe.playerPokemon[1].currHp < globe.playerPokemon[1].hp){
											globe.playerPokemon[1].currHp += 20;
											if(globe.playerPokemon[1].currHp > globe.playerPokemon[1].hp){
												globe.playerPokemon[1].currHp = globe.playerPokemon[1].hp;
											}
											potions--;
										}
									break;
									case 2:
										if(globe.playerPokemon[2].name != "empty"
										&& globe.playerPokemon[2].currHp < globe.playerPokemon[2].hp){
											globe.playerPokemon[2].currHp += 20;
											if(globe.playerPokemon[2].currHp > globe.playerPokemon[2].hp){
												globe.playerPokemon[2].currHp = globe.playerPokemon[2].hp;
											}
											potions--;
										}
									break;
									case 3:
										if(globe.playerPokemon[3].name != "empty"
										&& globe.playerPokemon[3].currHp < globe.playerPokemon[3].hp){
											globe.playerPokemon[3].currHp += 20;
											if(globe.playerPokemon[3].currHp > globe.playerPokemon[3].hp){
												globe.playerPokemon[3].currHp = globe.playerPokemon[3].hp;
											}
											potions--;
										}
									break;
									case 4:
										if(globe.playerPokemon[4].name != "empty"
										&& globe.playerPokemon[4].currHp < globe.playerPokemon[4].hp){
											globe.playerPokemon[4].currHp += 20;
											if(globe.playerPokemon[4].currHp > globe.playerPokemon[4].hp){
												globe.playerPokemon[4].currHp = globe.playerPokemon[4].hp;
											}
											potions--;
										}
									break;
									case 5:
										if(globe.playerPokemon[5].name != "empty"
										&& globe.playerPokemon[5].currHp < globe.playerPokemon[5].hp){
											globe.playerPokemon[5].currHp += 20;
											if(globe.playerPokemon[5].currHp > globe.playerPokemon[5].hp){
												globe.playerPokemon[5].currHp = globe.playerPokemon[5].hp;
											}
											potions--;
										}
									break;
								}
							break;
							case 'k':
								if(bagSelector > 0 ){
									bagSelector--;
								}
								validCommand = false;
							break;
							case 'j':
								if(bagSelector < 5){
									bagSelector ++;
								}
								validCommand = false;
							break;
							case 'Q':
								break;
							default:
								validCommand = false;
								break;
						}
					}else if(inMart || inCenter){
						if(commandShort == '<'){
							inMart = false;
							inCenter = false;
						}else if(commandShort == 'Q')
							break;
						validCommand = false;
					}else{
						switch(commandShort){
							case 'y':
								playerX--;
								playerY--;
								break;
							case 'k':
								playerY--;
								break;
							case 'u':
								playerX++;
								playerY--;
								break;
							case 'l':
								playerX++;
								break;
							case 'n':
								playerX++;
								playerY++;
								break;
							case 'j':
								playerY++;
								break;
							case 'b':
								playerX--;
								playerY++;
								break;
							case 'h':
								playerX--;
								break;
							case '7':
								playerX--;
								playerY--;
								break;
							case '8':
								playerY--;
								break;
							case '9':
								playerX++;
								playerY--;
								break;
							case '6':
								playerX++;
								break;
							case '3':
								playerX++;
								playerY++;
								break;
							case '2':
								playerY++;
								break;
							case '1':
								playerX--;
								playerY++;
								break;
							case '4':
								playerX--;
								break;
							case '>':
								if(!strcmp(currentMap->tiles[globe.playerY][globe.playerX],tile_str[CENTER])){
									inCenter = true;
									for(int i = 0;i < 6;i++){
										if(globe.playerPokemon[i].name != "empty"){
											globe.playerPokemon[i].currHp = globe.playerPokemon[i].hp;										}
									}
								}
								if(!strcmp(currentMap->tiles[globe.playerY][globe.playerX],tile_str[MART])){
									inMart = true;
									//resupply
									revives = 5;
									potions = 5;
									pokeballs = 5;
									
								}
								validCommand = false;
								break;
							case '.':
								break;
							case '5':
								break;
							case ' ':
								break;
							case 'B':
								showingOverworldbag = !showingOverworldbag;
								validCommand = false;
							break;
							case 't':
								showingList = !showingList;
								validCommand = false;
								break;
							case 'p':
								showingPoke = !showingPoke;
								validCommand = false;
								break;
							case 'Q':
								return;
							case 'f':
								fly = !fly;
								validCommand = true;
								break;
							default:
								validCommand = false;
								break;
						}
					}
					
				}
				if(getTileCost(currentMap->tiles[playerY][playerX],99) != INT16_MAX
				&& currentMap->chars[playerY][playerX].symbol == NULL){
					currentMap->chars[globe.playerY][globe.playerX].symbol = NULL;
					

					if(playerX == 0){
						//heap_insert(&currentMap->localHeap, &currentMap->chars[globe.playerY][globe.playerX]);
						posx--;
						playerX = MAPWIDTH-2;
						player.posX = playerX;
						player.posY = playerY;
						globe.playerX = playerX;
						globe.playerY = playerY;
						playerReturnToMapCalc(playerX,playerY);
						break;
					}
					else if(playerX == MAPWIDTH-1){
						//heap_insert(&currentMap->localHeap, &currentMap->chars[globe.playerY][globe.playerX]);
						posx++;
						playerX = 1;
						player.posX = playerX;
						player.posY = playerY;
						globe.playerX = playerX;
						globe.playerY = playerY;
						playerReturnToMapCalc(playerX,playerY);
						break;
					}
					else if(playerY == 0){
						//heap_insert(&currentMap->localHeap, &currentMap->chars[globe.playerY][globe.playerX]);
						posy--;
						playerY = MAPHEIGHT -2;
						player.posX = playerX;
						player.posY = playerY;
						globe.playerX = playerX;
						globe.playerY = playerY;
						playerReturnToMapCalc(playerX,playerY);
						break;
					}
					else if(playerY == MAPHEIGHT - 1){
						//heap_insert(&currentMap->localHeap, &currentMap->chars[globe.playerY][globe.playerX]);
						posy++;
						playerY = 1;
						player.posX = playerX;
						player.posY = playerY;
						globe.playerX = playerX;
						globe.playerY = playerY;
						playerReturnToMapCalc(playerX,playerY);
						break;
					}else{
						player.posX = playerX;
						player.posY = playerY;
						globe.playerX = playerX;
						globe.playerY = playerY;
						player.nextTurn += getTileCost(currentMap->tiles[playerY][playerX],99);
						currentMap->chars[playerY][playerX] = player;
						currentMap->chars[playerY][playerX].symbol = (char*)character_str[PLAYER];
					}

				}
				else if(getTileCost(currentMap->tiles[playerY][playerX],99) != INT16_MAX
				&& currentMap->chars[playerY][playerX].symbol != NULL
				&& currentMap->chars[playerY][playerX].symbol != character_str[PLAYER]
				&& !currentMap->chars[playerY][playerX].defeated){	
					opponent = &currentMap->chars[playerY][playerX];
					inBattle = true;
					while (inBattle)
					{
						handleBattle(currentMap->chars[playerY][playerX],0);
					}
				}
				else{
					player.posX = globe.playerX;
					player.posY = globe.playerY;
					player.nextTurn += getTileCost(currentMap->tiles[globe.playerY][globe.playerX],99);
					currentMap->chars[globe.playerY][globe.playerX] = player;
					currentMap->chars[globe.playerY][globe.playerX].symbol = (char*)character_str[PLAYER];

				}
					if(fly)
						break;
					

					//CHECK FOR LONG GRASS
					if(!strcmp(currentMap->tiles[globe.playerY][globe.playerX],tile_str[LONG])){
							int magicNumber = encounter(generator);//0-100
							if(magicNumber > 90){
								inEcounter = true;
								encounteredPoke = createPokemon(true);
							}

						}
						while(inEcounter){							
							handleBattle(*c,1);
						}
					
					calcCost(0,currentMap);calcCost(1,currentMap);
					currentMap->chars[globe.playerY][globe.playerX].nextTurn += getTileCost(currentMap->tiles[globe.playerY][globe.playerX],99);
					//currentTime = currentMap->chars[globe.playerY][globe.playerX].nextTurn;
					heap_insert(&currentMap->localHeap, &currentMap->chars[globe.playerY][globe.playerX]);
					break;
			}else if(!strcmp(c->symbol,character_str[HIKER])
			|| !strcmp(c->symbol,character_str[RIVAL])
			|| !strcmp(c->symbol,character_str[PACER])
			|| !strcmp(c->symbol,character_str[WANDERER])
			|| !strcmp(c->symbol,character_str[EXPLORERS])){
				if(c->defeated){
					//currentTime = currentMap->chars[c->posY][c->posX].nextTurn;
					//heap_insert(&charHeap, &currentMap->chars[c->posY][c->posX]);
				
				}else{
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

					//currentTime = currentMap->chars[c->nextY][c->nextX].nextTurn;
					//SET POKEMON HERE
					currentMap->chars[c->nextY][c->nextX].pokemon[0] = c->pokemon[0];
					currentMap->chars[c->nextY][c->nextX].pokemon[1] = c->pokemon[1];
					currentMap->chars[c->nextY][c->nextX].pokemon[2] = c->pokemon[2];
					currentMap->chars[c->nextY][c->nextX].pokemon[3] = c->pokemon[3];
					currentMap->chars[c->nextY][c->nextX].pokemon[4] = c->pokemon[4];
					currentMap->chars[c->nextY][c->nextX].pokemon[5] = c->pokemon[5];
					currentMap->chars[c->nextY][c->nextX].hn =
					heap_insert(&currentMap->localHeap, &currentMap->chars[c->nextY][c->nextX]);
					currentMap->chars[c->posY][c->posX].symbol = NULL;				
					break;
				}else if(currentMap->chars[c->nextY][c->nextX].symbol == character_str[PLAYER]){
					opponent = &currentMap->chars[c->posY][c->posX];
					inBattle = true;
					while (inBattle)
					{
						handleBattle(currentMap->chars[c->posY][c->posX],0);
					}
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
					
					//currentTime = currentMap->chars[c->posY][c->posX].nextTurn;
					//SET POKEMON HERE!
					currentMap->chars[c->posY][c->posX].pokemon[0] = c->pokemon[0];
					currentMap->chars[c->posY][c->posX].pokemon[1] = c->pokemon[1];
					currentMap->chars[c->posY][c->posX].pokemon[2] = c->pokemon[2];
					currentMap->chars[c->posY][c->posX].pokemon[3] = c->pokemon[3];
					currentMap->chars[c->posY][c->posX].pokemon[4] = c->pokemon[4];
					currentMap->chars[c->posY][c->posX].pokemon[5] = c->pokemon[5];
					heap_insert(&currentMap->localHeap, &currentMap->chars[c->posY][c->posX]);
					break;
				}
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
	if(numTrainers == 2){
		maxHiker = 1;
		maxRival = 1;
		totalMax = maxHiker + maxRival + maxPacer + maxWanderer+maxSenteries+maxExplorers;
	}else if(totalMax < numTrainers){
		maxHiker += numTrainers - totalMax;
		totalMax = maxHiker + maxRival + maxPacer + maxWanderer+maxSenteries+maxExplorers;
	}
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
				character hiker = character(x,y,(char*) character_str[HIKER],0,NULL,0,0);
				hiker.pokemon[0] = createPokemon(true);
				hiker.currentPoke = &hiker.pokemon[0];
				int numPoke = 1;
				while(numPoke < 6){
					int x = rand()%100;
					if(x >= 40){
						hiker.pokemon[numPoke] = createPokemon(true);
						numPoke++;
					}else{
						break;
					}
				}
				numHiker++;
				currentMap->chars[y][x] = hiker;
				setNextTurn(y,x,0,x,y);
				currentMap->chars[y][x].hn = 
				heap_insert(&currentMap->localHeap,&currentMap->chars[y][x]);
				
			}
		}
		if(numRival < maxRival){
				if(currentMap->chars[y][x].symbol == NULL){
				character rival = character(x,y,(char*) character_str[RIVAL],0,NULL,0,0);
				rival.pokemon[0] = createPokemon(true);
				rival.currentPoke = &rival.pokemon[0];
				int numPoke = 1;
				while(numPoke < 6){
					int x = rand()%100;
					if(x >= 40){
						rival.pokemon[numPoke] = createPokemon(true);
						numPoke++;
					}else{
						break;
					}
				}
				numRival++;
				currentMap->chars[y][x] = rival;
				setNextTurn(y,x,1,x,y);
				currentMap->chars[y][x].hn = 
				heap_insert(&currentMap->localHeap,&currentMap->chars[y][x]);
			}
		
		}//still breaks sometimes
		if(numPacer < maxPacer){
			if(currentMap->chars[y][x].symbol == NULL){
				character pacer = character(x,y,(char*) character_str[PACER],rand()%4,NULL,0,0);
				pacer.pokemon[0] = createPokemon(true);
				pacer.currentPoke = &pacer.pokemon[0];
				int numPoke = 1;
				while(numPoke < 6){
					int x = rand()%100;
					if(x >= 40){
						pacer.pokemon[numPoke] = createPokemon(true);
						numPoke++;
					}else{
						break;
					}
				}
				currentMap->chars[y][x]= pacer;
				setNextPace(y,x,y,x,pacer.dir,2);
				numPacer++;
				heap_insert(&currentMap->localHeap,&currentMap->chars[y][x]);
			}
			
		}
		if(numWanderer < maxWanderer){
			if(currentMap->chars[y][x].symbol == NULL){
				character wanderer = character(x,y,(char*) character_str[WANDERER],rand()%4,currentMap->tiles[y][x],0,0);
				wanderer.pokemon[0] = createPokemon(true);
				wanderer.currentPoke = &wanderer.pokemon[0];
				int numPoke = 1;
				while(numPoke < 6){
					int x = rand()%100;
					if(x >= 40){
						wanderer.pokemon[numPoke] = createPokemon(true);
						numPoke++;
					}else{
						break;
					}
				}
				currentMap->chars[y][x]= wanderer;
				setNextPace(y,x,y,x,wanderer.dir,3);
				numWanderer++;
				heap_insert(&currentMap->localHeap,&currentMap->chars[y][x]);
			}
			
		}
		if(numSentries < maxSenteries){
			if(currentMap->chars[y][x].symbol == NULL){
				character sentery = character(x,y,(char*) character_str[SENTRIES],0,NULL,0,0);
				sentery.pokemon[0] = createPokemon(true);
				sentery.currentPoke = &sentery.pokemon[0];
				int numPoke = 1;
				while(numPoke < 6){
					int x = rand()%100;
					if(x >= 40){
						sentery.pokemon[numPoke] = createPokemon(true);
						numPoke++;
					}else{
						break;
					}
				}
				currentMap->chars[y][x]= sentery;
				numSentries++;
			}	
		}
		if(numExplorers < maxExplorers){
			if(currentMap->chars[y][x].symbol == NULL){
				character explorer = character(x,y,(char*) character_str[EXPLORERS],rand()%4,NULL,0,0);
				explorer.pokemon[0] = createPokemon(true);
				explorer.currentPoke = &explorer.pokemon[0];
				int numPoke = 1;
				while(numPoke < 6){
					int x = rand()%100;
					if(x >= 40){
						explorer.pokemon[numPoke] = createPokemon(true);
						numPoke++;
					}else{
						break;
					}
				}
				currentMap->chars[y][x]= explorer;
				numExplorers++;		
				setNextPace(y,x,y,x,explorer.dir,4);
				heap_insert(&currentMap->localHeap,&currentMap->chars[y][x]);
			}
				
		}
	}
	
}
pokemonObject createPokemon(bool isEncounter){

	int randomPokemon = (rand() % pokemonDb.size() - 1) + 1;
	pokemon p = pokemonDb.at(randomPokemon);
	int distx, disty;
	distx = abs(posx - 200);
	disty = abs(posy - 200);
	int dist = distx + disty;
	int level, ivtotal = 0, hpiv, atkiv, defiv, spdiv, satkiv, sdefiv;
	int gender = rand() % 2;
	moves moveAr[4];
	bool shiny = false;
	if (rand() % 8192 == 1){shiny = true;}
	if (dist <= 200)
		level = (rand() % ((dist / 2) + 1)) + 1;
	else
		level = (dist - 200) / 2 + (rand() % (100 - ((dist - 200) / 2) + 1));
	// parse stats they are in order so its easy : )
	int values[6] = {0, 0, 0, 0, 0, 0};
	for (long unsigned int i = 0; i < pokemon_statsDb.size(); i++)
	{
		if (pokemon_statsDb.at(i).pokemon_id == p.id)
		{
			int iv = rand() % 16;
			if (pokemon_statsDb.at(i).stat_id == 1){
				values[0] = pokemon_statsDb.at(i).base_stat;
				hpiv = iv;}
			else if (pokemon_statsDb.at(i).stat_id == 2){
				values[1] = pokemon_statsDb.at(i).base_stat;
				atkiv = iv;}
			else if (pokemon_statsDb.at(i).stat_id == 3){
				values[2] = pokemon_statsDb.at(i).base_stat;
				defiv = iv;}
			else if (pokemon_statsDb.at(i).stat_id == 4){
				values[3] = pokemon_statsDb.at(i).base_stat;
				spdiv = iv;}
			else if (pokemon_statsDb.at(i).stat_id == 5){
				values[4] = pokemon_statsDb.at(i).base_stat;
				satkiv = iv;}
			else if (pokemon_statsDb.at(i).stat_id == 6){
				values[5] = pokemon_statsDb.at(i).base_stat;
				sdefiv = iv;}
			ivtotal += iv;
		}
		else if (pokemon_statsDb.at(i).pokemon_id > p.id)
		{
			break;
		}
	}
	int movesPicked = 0;
	std::vector<moves>possibleMoves;
	bool foundDupe = false;
	for (long unsigned int i = 0; i < pokemon_movesDb.size();i++){
		// moves need p_m.pid = p.sid and p_m.mmid = 1 and p_m.lvl <=  p.lvl
		// then do p_m.mid = m.id to get acutal move : )
		if(pokemon_movesDb.at(i).pokemon_id == p.species_id && pokemon_movesDb.at(i).pokemon_move_method_id == 1
		&&pokemon_movesDb.at(i).level <= level){
			for(long unsigned int j = 0; j < possibleMoves.size();j++){
				if(possibleMoves.at(j).id == pokemon_movesDb.at(i).move_id){
					foundDupe = true;
				}
			}
			if(!foundDupe)
				possibleMoves.push_back(movesDb.at(pokemon_movesDb.at(i).move_id-1));

			foundDupe = false;
		}

	}
	while(possibleMoves.size() < 2){
		//add struggle = id = 165 spot = ? probably 165
		possibleMoves.push_back(movesDb.at(164));
	}
	while(movesPicked != 4){
		//if !encounter pick 4 otherwise pick 2
		if(isEncounter && movesPicked >= 2){break;}
		int num = rand() % possibleMoves.size();
		moveAr[movesPicked] = possibleMoves.at(num);
		possibleMoves.erase(possibleMoves.begin()+num);
		movesPicked++;
	}
	
	std::string type = "empty";
	std::string type2 = "empty";
	for(long unsigned int i = 0; i < pokemon_typesDb.size();i++){
		if(pokemon_typesDb.at(i).pokemon_id == p.id){
			for(long unsigned int j = 0; j < type_namesDb.size();j++){
				if(pokemon_typesDb.at(i).type_id == type_namesDb.at(j).type_id
				&& type_namesDb.at(j).local_language_id == 9){
					if(type == "empty"){
						type = type_namesDb.at(j).name;
					}else{
						type2 = type_namesDb.at(j).name;
						break;
					}
				}
			}
		}
	}

	
	return pokemonObject(p.id, p.identifier, p.species_id, level, p.base_experience,
						 values[0], values[1], values[2], values[3], values[4], values[5], gender, ivtotal, shiny, moveAr, hpiv, atkiv, defiv, spdiv, satkiv, sdefiv,type,type2);
}
void levelUpPokemon(pokemonObject poke){

}
int calcDamage(moves pokeMove,pokemonObject attacker,pokemonObject defender){
	int result = 0;
	int crit = rand()%255;
	double critValue = 1;
	if(crit <= (attacker.spd/2)){critValue = 1.5;}	
	double random = rand()%15;random+=85;
	random /= 100;
	random = round(random);
	double stab = 1;
	if(attacker.type == defender.type || attacker.type == defender.type2
	||attacker.type2 == defender.type || attacker.type2 == defender.type2){
		stab = 1.5;
	}
	int power = pokeMove.power;
	if(power == INT_MAX){power = 1;}
	result = ((((((2*attacker.level)/5)+2) * power * (attacker.curratk/defender.currdef))/50)+2)*critValue*random*stab;
	return result;
}
void playerMove(pokemonObject &poke,int moveIndex,bool skipPlayer){
	std::string message1,message2;

	bool trainerHit = true;
	bool playerHit = true;
	int move = rand() % 2;
	int trainerChance = rand() % 100;
	int playerChance = rand()%100;
	
	if(trainerChance <= poke.availableMoves[move].accuracy){
		message1 = poke.name + " used " +poke.availableMoves[move].indentifier+"!";
	}else{
		message1 = poke.name + " missed!";
		trainerHit = false;
	}
	if(playerChance <= globe.playerCurrentPokemon->availableMoves[moveIndex].accuracy && !skipPlayer){
		message2 = globe.playerCurrentPokemon->name + " used " +globe.playerCurrentPokemon->availableMoves[moveIndex].indentifier+"!";
	}else if(skipPlayer){
		message2 = "player used item/switched poke";
		playerHit = false;
	}else{
		message2 = globe.playerCurrentPokemon->name + "missed!";
		playerHit = false;
	}
	
	
	if(globe.playerCurrentPokemon->availableMoves[moveIndex].priority >= poke.availableMoves[move].priority){
		if(globe.playerCurrentPokemon->availableMoves[moveIndex].priority== poke.availableMoves[move].priority){
			if(globe.playerCurrentPokemon->spd >= poke.spd){
				battleMessage = message2;
				if(playerHit)
					poke.currHp -= calcDamage(globe.playerCurrentPokemon->availableMoves[moveIndex],*globe.playerCurrentPokemon,poke);
				if(poke.currHp <= 0){return;}
				printMap(currentMap);
				battleMessage = message1;
				if(trainerHit)
					globe.playerCurrentPokemon->currHp -= calcDamage(poke.availableMoves[move],poke,*globe.playerCurrentPokemon);
				if(globe.playerCurrentPokemon->currHp <= 0){return;}
				printMap(currentMap);
			}else{
				battleMessage = message1;
				if(trainerHit)
					globe.playerCurrentPokemon->currHp -= calcDamage(poke.availableMoves[move],poke,*globe.playerCurrentPokemon);
				if(globe.playerCurrentPokemon->currHp <= 0){return;}
				printMap(currentMap);
				battleMessage = message2;
				if(playerHit)
					poke.currHp -= calcDamage(globe.playerCurrentPokemon->availableMoves[moveIndex],*globe.playerCurrentPokemon,poke);
				if(poke.currHp <= 0){return;}
				printMap(currentMap);
			}
		}else{
			battleMessage = message2;
			if(playerHit)
				poke.currHp -= calcDamage(globe.playerCurrentPokemon->availableMoves[moveIndex],*globe.playerCurrentPokemon,poke);
			if(poke.currHp <= 0){return;}
			printMap(currentMap);
			battleMessage = message1;
			if(trainerHit)
				globe.playerCurrentPokemon->currHp -= calcDamage(poke.availableMoves[move],poke,*globe.playerCurrentPokemon);
			if(globe.playerCurrentPokemon->currHp <= 0){return;}
			printMap(currentMap);
		}
	}else{
		battleMessage = message1;
		if(trainerHit)
			globe.playerCurrentPokemon->currHp -= calcDamage(poke.availableMoves[move],poke,*globe.playerCurrentPokemon);
		if(globe.playerCurrentPokemon->currHp <= 0){return;}
		printMap(currentMap);
		battleMessage = message2;
		if(playerHit)
			poke.currHp -= calcDamage(globe.playerCurrentPokemon->availableMoves[moveIndex],*globe.playerCurrentPokemon,poke);
		if(poke.currHp <= 0){return;}
		printMap(currentMap);	
	}

	
	

}
void handleBattle(character_c &trainer,int encounter){
	if(!encounter){
		for(int i = 0; i < 6;i++){ //get first poke with hp
			if(trainer.pokemon[i].name != "empty"){
				if(trainer.pokemon[i].currHp > 0){
					trainer.currentPoke = &trainer.pokemon[i];
					break;
				}
			}
			if(i == 5){// all dead
				inBattle = false;
				inEcounter = false;
				battleSelector = 0;
				playerBattleTurn = true;
				trainer.defeated = true;
				return;
			}
		}
	}else{
		if(encounteredPoke.currHp <= 0 ){
			inEcounter = false;
				battleSelector = 0;
				playerBattleTurn = true;
				return;
		}
	}
	if(globe.playerCurrentPokemon->currHp <= 0){
		for(int i = 0; i < 6;i++){ //get first poke with hp
			if(globe.playerPokemon[i].name != "empty"){
				if(globe.playerPokemon[i].currHp > 0){
					globe.playerCurrentPokemon = &globe.playerPokemon[i];
					break;
				}
			}
			if(i == 5){//player all dead
				battleMessage = "all your pokemon are dead lol";
				printMap(currentMap);
				inBattle = false;
				inEcounter = false;
				battleSelector = 0;
				playerBattleTurn = true;
				return;
			}
		}
	}			
	printMap(currentMap);
	commandShort = getch();
	if (commandShort == KEY_ESC)
	{
		inBattle = false;
		inEcounter = false;
		battleSelector = 0;
		playerBattleTurn = true;
		showingBag = false;
		showingMoves = false;
		showingSwitchPoke = false;
		if(!encounter)
			trainer.defeated = true;
		return;
	}else if(commandShort == 'Q'){
		inBattle = false;
		inEcounter = false;
		battleSelector = 0;
		playerBattleTurn = true;
		battleMessage = "empty";
		return;
	}else if(commandShort == 'k'){		
		if(battleSelector > 0 ){
			battleSelector--;
		}
	}else if(commandShort == 'j'){
		if(showingSwitchPoke){
			if(battleSelector < 5){
				battleSelector++;
			}
		}else{
			if(battleSelector < 3){
				battleSelector++;
			}
		}
		
	}else if(commandShort == 'f' || commandShort == 'F'){
		//select the thing its on
		if(showingMoves){
			showingMoves = !showingMoves;
			if(!encounter)
				playerMove(*trainer.currentPoke,battleSelector,false);
			else
				playerMove(encounteredPoke,battleSelector,false);
			if(globe.playerCurrentPokemon->currHp <= 0){ 
				for(int i = 0; i < 6;i++){ //get first poke with hp
					if(globe.playerPokemon[i].name != "empty"){
						if(globe.playerPokemon[i].currHp > 0){
							break;
						}
					}
					if(i == 5){//player all dead
						inBattle = false;
						inEcounter = false;
						battleSelector = 0;
						playerBattleTurn = true;
						battleMessage = "empty";
						return;
					}
				}
				battleMessage = globe.playerCurrentPokemon->name + " fainted! :(";
				printMap(currentMap);
			}
			battleSelector = 0;
		}else if(showingBag){
			switch (battleSelector)
				{
					case 0:
					reviving = true;
					showingBag = false;
					showingSwitchPoke = true;
						break;
					case 1:
					//potion
					globe.playerCurrentPokemon->currHp += 20;
					potions--;
					if(globe.playerCurrentPokemon->currHp > globe.playerCurrentPokemon->hp){
						globe.playerCurrentPokemon->currHp = globe.playerCurrentPokemon->hp;
					}
					if(!encounter)
						playerMove(*trainer.currentPoke,battleSelector,true);
					else
						playerMove(encounteredPoke,battleSelector,true);
					showingBag = false;
					battleSelector = 0;
						break;
					case 2:
					if(encounter){
						for(int i = 0;i<6;i++){
							if(globe.playerPokemon[i].name == "empty"){
								globe.playerPokemon[i] = encounteredPoke;
								battleMessage = encounteredPoke.name + " has been caught!!";
								printMap(currentMap);
								pokeballs--;
								inEcounter = false;
								break;
							}
							if(i == 5){
								battleMessage = encounteredPoke.name + " ran away!! :(";
								printMap(currentMap);
								inEcounter = false;
							}
						}
						battleSelector = 0;
					}
					showingBag = false;
					break;
				}	
		}else if(showingSwitchPoke){
			if(!reviving){
				if(globe.playerPokemon[battleSelector].currHp > 0 && globe.playerPokemon[battleSelector].name != "empty"){
					globe.playerCurrentPokemon = &globe.playerPokemon[battleSelector];
					printMap(currentMap);
					if(!encounter)
						playerMove(*trainer.currentPoke,battleSelector,true);
					else
						playerMove(encounteredPoke,battleSelector,true);
					battleSelector = 0;
				}
			}else{
				if(globe.playerPokemon[battleSelector].currHp <= 0 && globe.playerPokemon[battleSelector].name != "empty"){
					globe.playerPokemon[battleSelector].currHp = globe.playerPokemon[battleSelector].hp;
					revives--;
					if(!encounter)
						playerMove(*trainer.currentPoke,battleSelector,true);
					else
						playerMove(encounteredPoke,battleSelector,true);
					battleSelector = 0;
				}
				reviving = false;
			}
			showingSwitchPoke = !showingSwitchPoke;	
		}
		else{
			switch (battleSelector)
				{
					case 0:
					showingMoves = !showingMoves;
					battleSelector = 0;
					//fight
						break;
					case 1:
					//bag
					showingBag = !showingBag;
					battleSelector = 0;
						break;
					case 2://run
					if(encounter){
						inEcounter = false;
						battleSelector = 0;
						battleMessage = "empty";
						playerBattleTurn = true;
					}
					break;
					case 3:
					showingSwitchPoke = !showingSwitchPoke;
					battleSelector = 0;
					//pokemon
						break;
				}	
		}

	}
}