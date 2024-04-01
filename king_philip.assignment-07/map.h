#ifndef MAP_H
#define MAP_H
#define MAPWIDTH 80
#define MAPHEIGHT 21
#include "heap.h"
#include "character.h"
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

    void initMap(int a,int b,int c, int d){}
    void findPath(int x ,int y, int targetx, int targety){}
    void genPaths(int new_a, int new_b, int new_c, int new_d){}
    void genBuildings(){}   
	//int *otherCostMap[MAPHEIGHT][MAPWIDTH]; same as rivial for the time being
};

#endif