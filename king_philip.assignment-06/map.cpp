#include "map.h"
#include "poke.cpp"
char *oldMap[MAPHEIGHT][MAPWIDTH];
int heightmap[MAPHEIGHT][MAPWIDTH];
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
	copyMap(globe.maps[posy][posx],currentMap);
	free(currentMap);
}