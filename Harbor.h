#pragma once
#include "defines.h"

class Harbor {
    friend class Map;
private:
    // 固定数据
    int Id;
    int x, y;
    int time;
    int velocity;
public:
    // 动态数据
    vector<int> productPrices;
    // 私有函数

public:
    Harbor(int _Id, int _x, int _y, int _time, int _velocity);
    Harbor();
    // 公共函数
};