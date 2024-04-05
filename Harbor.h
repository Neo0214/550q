#pragma once
#include "defines.h"
#include "Order.h"
#include "Boat.h"
#include "args.h"
#include "Coord.h"

class Harbor {
    friend class Map;
    friend class Scheduler;
private:
    // �̶�����
    int Id;
    int x, y;
    int leftTopX, leftTopY;
    int velocity;
    Coord berthCoord; // ͣ��Ŀ������

    float area;
public:
    // ��̬����
    vector<int> productPrices;
    vector<Order> orders; // ��¼�ۿ��Ѿ�������Ķ���
    float endingCost = 1; // �����ʱ���Ƿ�ѡ��
    int totalReceived = 1;
    bool tiger = false;
    double prioTiger = 1;

    vector<double> expectedProfitRate; // �ۿڸ�������Ʒ�������ʣ��Ӵ�С��
    vector<int> robotsGoalHarbor; // ����øۿڵĻ�����
    // ˽�к���

public:
    Harbor(int _Id, int _x, int _y, int _velocity);
    Harbor();
    // ��������
    void getBestCoord(int points[LEN][LEN]);
    void clearOneOrder();
    void addOneOrder();
    void newOrder(int goodsLeft, int boatId);
    double getExpectedProfitRate(int robotId,double exceptProfit);
    void appendRobot(int robotId);
    void removeRobot(int robotId);
    void appendProfitRate(double profitRate);
    void removeProfitRate(double profitRate);
    Coord getPos();
    Coord getBerthCoord();
    void setBerthCoord(Coord coord);
};