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

    int harborVelIndex[10]; // �ۿ��ٶ���������
    int boatCapacity; // ��������
    // ��̬����
    int frame; // ��ǰ֡��

    // ˽�к���
    void findHarbor(int robotId); //������ĸۿ�
    void findProductAndHarbor(int robotId); //����������ߵ���Ʒ�Ͷ�Ӧ�ۿ�
    void initHarbor(); // �Ըۿ��ٶ�����ʼ������
    vector<int> getFreeBoat(); // ��ȡ���д�ֻ
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