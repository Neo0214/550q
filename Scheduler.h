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

    int harborVelIndex[10]; // 港口速度排序索引
    int boatCapacity; // 船的容量
    // 动态数据
    int frame; // 当前帧号

    // 私有函数
    void findHarbor(int robotId); //找最近的港口
    void findProductAndHarbor(int robotId); //找收益率最高的物品和对应港口
    void initHarbor(); // 对港口速度做初始化处理
    vector<int> getFreeBoat(); // 获取空闲船只
    int selectFastestHarbor(int countNeeded);
    int selectAvailableFastestHarborWithGoingFromOriginPoint();
    void loadGoods(Boat* _boat,Harbor* _harbor);
    int getValue(int harborId);
    int getFutureValue(int harborId, int total);
    
public:
    Scheduler();

    bool NextFrame();
    void Update();
    void printValue();
};