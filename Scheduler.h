#pragma once
#include "defines.h"
#include "Map.h"
#include "Harbor.h"
#include "Boat.h"
#include "Robot.h"
#include "Product.h"
#include "PathPlanner.h"

class Scheduler {
private:
    // 固定数据
    Map map;
    Harbor harbor[10];
    Boat boat[5];
    Robot robot[10];
    vector<Product> products;
    
    PathPlanner pathPlanner; // 路径规划器

    // 动态数据
    int frame; // 当前帧号

    // 私有函数
    void findHarbor(int robotId); //找最近的港口
    void findProductAndHarbor(int robotId); //找收益率最高的物品和对应港口


public:
    Scheduler();

    bool NextFrame();
    void Update();
};