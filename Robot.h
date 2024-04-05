#pragma once
#include "defines.h"
#include "Product.h"
#include "Coord.h"
#define ON 1
#define OFF 0

#define NO_GOODS 0
#define HAS_GOODS 1

class Robot {
public:
	// 固定数据
	int id;
	bool hasGoods;
	int x, y;
	int status;
	// 动态数据
	vector<int> moves; // 未来的移动
	int curMoveCount=0; // 当前移动的步数
	int target=1; 
	int atHarbor=-1; // 目标港口
	int goalHarbor = -1; // 所在港口
	int carryProduct = -1;

	// 私有函数
public:
	Robot(int id, int _x, int _y);
	Robot() {};
	// 公共函数
	void update(int hasGoods, int x, int y);
	void assignTask(const vector<int>& moves, int target, int atHarbor, int goalHarbor);
	pair<int, vector<int>> moveOneStep(int collisionMap[LEN][LEN]);
	void redoOneStep(int collisionMap[LEN][LEN],int originMove);
	int command(int realMove);
	Coord getNextPos();
};

