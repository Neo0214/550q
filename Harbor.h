#pragma once
#include "defines.h"

class Harbor {
    friend class Map;
private:
    // �̶�����
    int Id;
    int x, y;
    int time;
    int velocity;
public:
    // ��̬����
    vector<int> productPrices;
    // ˽�к���

public:
    Harbor(int _Id, int _x, int _y, int _time, int _velocity);
    Harbor();
    // ��������
};