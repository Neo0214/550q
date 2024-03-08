#pragma once
#include "defines.h"



class Product
{
public:
	int x;
	int y;
	int expireTime;
	int price;

	// 前往每个港口的距离
	int distanceToHarbors[HARBOR_NUM];
public:
	Product(int x, int y, int price, int expireTime, int distanceToHarbors[HARBOR_NUM]);
	Product() {};
	int getNearestHarborId();

};