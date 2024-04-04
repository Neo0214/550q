#pragma once
#include "defines.h"
#include "Order.h"

#define MOVING 0
#define NORMAL 1
#define WAITING 2

class Boat {
    friend class Scheduler;
private:
    // 固定数据
    int id;
    int capacity;
    // 动态数据
    int status;
    Coord pos;
    int direction;
    int curCapacity;
    int curValue;
    int preLoadNum;
    vector<Order> orders;
    int turns;
    bool atLast = false;
    // 私有函数

public:
    Boat(int _id, int _capacity);
    Boat();
    // 公共函数
    void update(int status, int curCapacity, int x, int y, int direction);
    void gotoHarbor(int harborId);
    void comeBack(int frame);
    int getStatus();
    int getPos();
    int getCurCapacity();
    void addGoods(int num, int value);
    int getCurValue();
    int getPre();
    void clearOrders();
    void clearOneOrder();
    void addOneOrder();
    int getOrderCapacity();
    void newOrder(int goodsLeft, int harborId);
    int originPos();
};