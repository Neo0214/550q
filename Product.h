#pragma once
#include "defines.h"


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
	int distanceToHarbors[HARBOR_NUM];
public:
	Product(int x, int y, int price, int expireTime, int distanceToHarbors[HARBOR_NUM]);
	Product() {};

	// ��ȡ����ĸۿ�Id��-1��ʾû�пɴ�ۿ�
	int getNearestHarborId();

};