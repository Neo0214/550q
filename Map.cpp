#include "defines.h"

short* Map::getMoveCommand(int harborId, int destX, int destY)
{
	return NULL;
}

Map::Map()
{
	memset(point, 0, sizeof(point));
}

void Map::setPoint(int x, int y, int type)
{
	point[x][y] = type;
}

void Map::setGoods(int x, int y, int val, int overFrame)
{
	/* int
	*  31-24 23-16    15-8 7-0
	*          val   overFrame  
	*/
	point[x][y]=(val<<16)|overFrame;
}