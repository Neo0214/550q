#pragma once
#include "defines.h"

#define MOVING 0
#define NORMAL 1
#define WAITING 2

class Boat {
private:
    // �̶�����
    int id;
    int capacity;
    // ��̬����
    int status;
    int pos;
    // ˽�к���

public:
    Boat(int _id, int _capacity);
    Boat();
    // ��������
    void update(int status, int pos);
};