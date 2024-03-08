#pragma once
#include "defines.h"
#include "Map.h"
#include "Harbor.h"
#include "Boat.h"
#include "Robot.h"
#include "PathPlanner.h"

class Scheduler {
private:
    // 固定数据
    Map map;
    Harbor harbor[10];
    Boat boat[5];
    Robot robot[10];
    
    PathPlanner pathPlanner;
    // 动态数据
    int frame; // 当前帧号
    // 私有函数
    void writeCommand();
public:
    Scheduler();
    // 公共函数
    bool NextFrame();
    void Update();
};