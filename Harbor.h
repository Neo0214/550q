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
    // ��̬����

    // ˽�к���

public:
    Harbor(int _Id, int _x, int _y, int _time, int _velocity);
    Harbor();
    // ��������
};