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
    // �̶�����
    Map map;
    Harbor harbor[10];
    Boat boat[5];
    Robot robot[10];
    vector<Product> products;
    
    PathPlanner pathPlanner;
    // ��̬����
    int frame; // ��ǰ֡��
    // ˽�к���
    void writeCommand();
    void RobotCommand(string command,int id, int direct=-1);

    void findProductAndHarbor(int robotId);

public:
    Scheduler();
    // ��������
    bool NextFrame();
    void Update();
};