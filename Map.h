#pragma once
#include "defines.h"

#define SEA -3
#define WALL -2
#define HARBOR -1
#define EMPTY 0

class Map {
public:
    // �̶�����
    /*
    * 0 �յ�
    * 1 ����
    * 2 �ϰ�
    * 3 ������
    * 4 �ۿ�
    */
    int point[200][200];
    // ��̬����

    // ˽�к���

public:
    Map();
    // ��������
    void setGoods(int x, int y, int val, int overFrame);
    void setPoint(int x, int y, int type);
};