#pragma once
#include "defines.h"

#define SEA -3
#define WALL -2
#define HARBOR -1
#define EMPTY 0

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