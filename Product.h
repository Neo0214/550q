#pragma once
#include "defines.h"
#include "Coord.h"

class Product
{
public:
	int x;
	int y;
	int expireTime; //����ʱ�䣨֡��
	int price;

	int nearestHarborId;
	double bestProfitRate;
	// ��̬����
	bool locked = false; // �Ƿ�����

	// ǰ��ÿ���ۿڵľ���
	vector<int> distanceToHarbors;
public:
	Product(int x, int y, int price, int expireTime, const vector<int>& distanceToHarbors);
	Product() {};

	// ��ȡ����ĸۿ�Id��-1��ʾû�пɴ�ۿ�
	int getNearestHarborId();

};