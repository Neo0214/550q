#pragma once
#include "defines.h"

class Harbor {
    friend class Map;
    friend class Scheduler;
private:
    // �̶�����
    int Id;
    int x, y;
    int time;
    int velocity;
public:
    // ��̬����
    vector<int> productPrices;
    int leftGoodsNumber; // ͨ������£���Ӧ����0�������д��ڴ˴�װ�أ��Ų���
    // ˽�к���

public:
    Harbor(int _Id, int _x, int _y, int _time, int _velocity);
    Harbor();
    // ��������
};