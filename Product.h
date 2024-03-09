#pragma once
#include "defines.h"


class Product
{
public:
	int x;
	int y;
	int expireTime; //����ʱ�䣨֡��
	int price;

	// ǰ��ÿ���ۿڵľ���
	int distanceToHarbors[HARBOR_NUM];
public:
	Product(int x, int y, int price, int expireTime, int distanceToHarbors[HARBOR_NUM]);
	Product() {};

	// ��ȡ����ĸۿ�Id
	int getNearestHarborId();

};