#pragma once
#define _CRT_SECURE_NO_WARNINGS


#include <bits/stdc++.h>

#define HARBOR_NUM 10 // 港口数量
#define ROBOT_NUM 10
#define LEN 200 // 地图边长

using namespace std;
constexpr int reverseMove[4] = { 1,0,3,2 }; // 移动方向反转数组
struct Coord
{
	int x;
	int y;
	Coord(int x, int y) :x(x), y(y) {};
	Coord() {};
	Coord operator+(int move) {
		switch (move)
		{
		case 0:
			return Coord(x, y + 1);
		case 1:
			return Coord(x, y - 1);
		case 2:
			return Coord(x - 1, y);
		case 3:
			return Coord(x + 1, y);
		}

		return Coord();

	}
	bool operator==(const Coord& c) const {
		return x == c.x && y == c.y;
	}
};