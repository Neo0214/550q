#pragma once
#include "defines.h"
#include "Map.h"
#include "Harbor.h"
#include "Boat.h"
#include "Robot.h"
#include "PathPlanner.h"

class Scheduler {
private:
    // �̶�����
    Map map;
    Harbor harbor[10];
    Boat boat[5];
    Robot robot[10];
    
    PathPlanner pathPlanner;
    // ��̬����
    int frame; // ��ǰ֡��
    // ˽�к���
    void writeCommand();
public:
    Scheduler();
    // ��������
    bool NextFrame();
    void Update();
};