#pragma once
#include "defines.h"
#include "Coord.h"

class Product
{
public:
	int x;
	int y;
	int expireTime; //到期时间（帧）
	int price;

	int nearestHarborId;
	double bestProfitRate;
	// 动态数据
	bool locked = false; // 是否被锁定

	// 前往每个港口的距离
	vector<int> distanceToHarbors;
public:
	Product(int x, int y, int price, int expireTime, const vector<int>& distanceToHarbors);
	Product() {};

	// 获取最近的港口Id，-1表示没有可达港口
	int getNearestHarborId();

};