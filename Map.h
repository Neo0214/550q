#pragma once
#include "defines.h"
#include "Coord.h"

class Map {
public:
    // �̶�����
    char point[200][200];
    // ��̬����

    // ˽�к���

public:
    Map();
    // ��������
    void setGoods(int x, int y, int val, int overFrame);
    void setPoint(int x, int y, char type);
};