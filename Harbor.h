#pragma once
#include "defines.h"
#include "Order.h"
#include "Boat.h"

class Harbor {
    friend class Map;
    friend class Scheduler;
private:
    // 固定数据
    int Id;
    int x, y;
    int leftTopX, leftTopY;
    int time;
    int velocity;

    float area;
public:
    // 动态数据
    vector<int> productPrices;
    vector<Order> orders; // 记录港口已经被分配的订单
    float endingCost = 1; // 快结束时，是否被选中
    // 私有函数

public:
    Harbor(int _Id, int _x, int _y, int _time, int _velocity);
    Harbor();
    // 公共函数
    void getBestCoord(int points[LEN][LEN]);
    void clearOneOrder();
    void addOneOrder();
    void newOrder(int goodsLeft, int boatId);
};