#pragma once
#include "defines.h"
#include "Order.h"

#define MOVING 0
#define NORMAL 1
#define WAITING 2

class Boat {
    friend class Scheduler;
private:
    // �̶�����
    int id;
    int capacity;
    // ��̬����
    int status;
    int pos;
    int curCapacity;
    int curValue;
    int preLoadNum;
    vector<Order> orders;
    int turns;
    bool atLast = false;
    // ˽�к���

public:
    Boat(int _id, int _capacity);
    Boat();
    // ��������
    void update(int status, int pos);
    void gotoHarbor(int harborId);
    void comeBack(int frame);
    int getStatus();
    int getPos();
    int getCurCapacity();
    void addGoods(int num, int value);
    int getCurValue();
    int getPre();
    void clearOrders();
    void clearOneOrder();
    void addOneOrder();
    int getOrderCapacity();
    void newOrder(int goodsLeft, int harborId);
    int originPos();
};