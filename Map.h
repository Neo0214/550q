#pragma once
#include "defines.h"


class Map {
public:
    // �̶�����
    int point[200][200];
    // ��̬����

    // ˽�к���

public:
    Map();
    // ��������
    void setGoods(int x, int y, int val, int overFrame);
    void setPoint(int x, int y, int type);
};