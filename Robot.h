#pragma once
#include "defines.h"

#define ON 1
#define OFF 0

#define NO_GOODS 0
#define HAS_GOODS 1

class Robot {
public:
	// 固定数据
	bool hasGoods;
	int x, y;
	int status;
	// 动态数据
	vector<int> moves; // 未来的移动
	int curMoveCount; // 当前移动的步数
	int target; // -1: 无目标，其他：前往港口的编号
	int atHarbor; // -1: 不在港口，其他：在港口的编号

	// 私有函数
public:
	Robot(int _x, int _y);
	Robot();
	// 公共函数
	void update(int hasGoods, int x, int y, int status);
	void assignTask(const vector<int>& moves, int target);
};

