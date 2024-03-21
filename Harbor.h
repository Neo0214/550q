#pragma once
#include "defines.h"
#include "Order.h"
#include "Boat.h"

class Harbor {
    friend class Map;
    friend class Scheduler;
private:
    // �̶�����
    int Id;
    int x, y;
    int leftTopX, leftTopY;
    int time;
    int velocity;

    float area;
public:
    // ��̬����
    vector<int> productPrices;
    vector<Order> orders; // ��¼�ۿ��Ѿ�������Ķ���
    float endingCost = 1; // �����ʱ���Ƿ�ѡ��
    // ˽�к���

public:
    Harbor(int _Id, int _x, int _y, int _time, int _velocity);
    Harbor();
    // ��������
    void getBestCoord(int points[LEN][LEN]);
    void clearOneOrder();
    void addOneOrder();
    void newOrder(int goodsLeft, int boatId);
};