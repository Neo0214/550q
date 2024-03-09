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
    
    PathPlanner pathPlanner; // ·���滮��

    // ��̬����
    int frame; // ��ǰ֡��

    // ˽�к���
    void findHarbor(int robotId); //������ĸۿ�
    void findProductAndHarbor(int robotId); //����������ߵ���Ʒ�Ͷ�Ӧ�ۿ�


public:
    Scheduler();

    bool NextFrame();
    void Update();
};