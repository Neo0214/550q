#pragma once
#define _CRT_SECURE_NO_WARNINGS
// #define DEBUG

#include <bits/stdc++.h>

#define HARBOR_NUM 10 // 港口数量
#define ROBOT_NUM 2
#define LEN 200 // 地图边长

using namespace std;

struct Coord
{
	int x;
	int y;
	Coord(int x, int y) :x(x), y(y) {};
	Coord() {};
};