#pragma once
#include "defines.h"
#include "Product.h"

#define ON 1
#define OFF 0

#define NO_GOODS 0
#define HAS_GOODS 1

class Robot {
public:
	// �̶�����
	int id;
	bool hasGoods;
	int x, y;
	int status;
	// ��̬����
	vector<int> moves; // δ�����ƶ�
	int curMoveCount=0; // ��ǰ�ƶ��Ĳ���
	int target=1; 
	int atHarbor=-1; // Ŀ��ۿ�
	int carryProduct = -1;

	// ˽�к���
public:
	Robot(int id, int _x, int _y);
	Robot() {};
	// ��������
	void update(int hasGoods, int x, int y, int status);
	void assignTask(const vector<int>& moves, int target, int harbor);
	pair<int, vector<int>> moveOneStep(int collisionMap[LEN][LEN]);
	void redoOneStep(int collisionMap[LEN][LEN],int originMove);
	int command(int realMove);
	Coord getNextPos();
};

