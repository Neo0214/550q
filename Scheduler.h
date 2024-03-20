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

    int boatCapacity; // ��������
    int minBackTime; // ��С�ظ�ʱ��
    // ��̬����
    int frame; // ��ǰ֡��
    int score = 0;
    vector<int> harborWhoGotReceive; // ��¼�ĸ��ۿ�����һ֡�н����˻���

    // ˽�к���
    void findHarbor(int robotId); //������ĸۿ�
    void findProductAndHarbor(int robotId); //����������ߵ���Ʒ�Ͷ�Ӧ�ۿ�
    vector<int> getFreeBoat(); // ��ȡ���д�ֻ
    int selectFastestHarbor(int countNeeded, int timeTocomeBack, int holdedValue);
    int selectAvailableFastestHarborWithGoingFromOriginPoint();
    void loadGoods(Boat* _boat,Harbor* _harbor);
    //int getValue(int harborId);
    float getFutureValue(int harborId, int total);
    int getFutureValueFromOriginPoint(int harborId, int total);
    void clearWhenBoatComeBack(int boatId, int harborId);
    void clearWhenBoatSwitch(int boatId, int harborId);
    int hasBoat(int harborId);
    void synchronizeWhenGoOut(int boatId, int harborId);
    void synchronizeWhenSwitch(int boatId, int harborId);
public:
    Scheduler();

    bool NextFrame();
    void Update();
    void printValue();
};