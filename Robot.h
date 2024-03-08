#pragma once
#include "defines.h"

#define ON 1
#define OFF 0

#define NO_GOODS 0
#define HAS_GOODS 1

class Robot {
public:
	// �̶�����
	bool hasGoods;
	int x, y;
	int status;
	// ��̬����
	vector<int> moves; // δ�����ƶ�
	int curMoveCount; // ��ǰ�ƶ��Ĳ���
	int target; // -1: ��Ŀ�꣬������ǰ���ۿڵı��
	int atHarbor; // -1: ���ڸۿڣ��������ڸۿڵı��

	// ˽�к���
public:
	Robot(int _x, int _y);
	Robot();
	// ��������
	void update(int hasGoods, int x, int y, int status);
	void assignTask(const vector<int>& moves, int target);
};

