#pragma once
#include "defines.h"

#define ON 1
#define OFF 0

#define NO_GOODS 0
#define HAS_GOODS 1

class Robot {
private:
	// �̶�����
	int goods;
	int x, y;
	int status;
	// ��̬����

	// ˽�к���
public:
	Robot(int _x, int _y);
	Robot();
	// ��������
	void update(int hasGoods, int x, int y, int status);
};

