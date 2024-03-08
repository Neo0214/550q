#pragma once
#include "defines.h"

#define ON 1
#define OFF 0

#define NO_GOODS 0
#define HAS_GOODS 1

class Robot {
private:
	// 固定数据
	int goods;
	int x, y;
	int status;
	// 动态数据

	// 私有函数
public:
	Robot(int _x, int _y);
	Robot();
	// 公共函数
	void update(int hasGoods, int x, int y, int status);
};

