#pragma once
#include "defines.h"
#include "Coord.h"

class Map {
public:
    // 固定数据
    char point[200][200];
    // 动态数据

    // 私有函数

public:
    Map();
    // 公共函数
    void setGoods(int x, int y, int val, int overFrame);
    void setPoint(int x, int y, char type);
};