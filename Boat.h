#pragma once
#include "defines.h"

#define MOVING 0
#define NORMAL 1
#define WAITING 2

class Boat {
private:
    // 固定数据
    int id;
    int capacity;
    // 动态数据
    int status;
    int pos;
    // 私有函数

public:
    Boat(int _id, int _capacity);
    Boat();
    // 公共函数
    void update(int status, int pos);
};