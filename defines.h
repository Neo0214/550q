#pragma once
#define _CRT_SECURE_NO_WARNINGS


#include <bits/stdc++.h>

#define LEN 200 // 地图边长


#define EMPTY 0
#define HARBOR -1


// 地图输入数据
#define EMPTY_SPACE '.'
#define MAIN_LAND '>'
#define SEA_SPACE '*'
#define MAIN_SEA '~'
#define BLOCK '#'
#define BUY_ROBOT_SPACE 'R'
#define BUY_SHIP_SPACE 'S'
#define HARBOR_SPACE 'B'
#define LOAD_SPACE 'K'
#define OVERPASS 'C'
#define OVERPASS_MAIN 'c'
#define DELIVERY 'T'

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
		case -1:
			return Coord(x, y);
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