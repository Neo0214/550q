#include "Product.h"

Product::Product(int x, int y, int price, int expireTime, int distanceToHarbors[HARBOR_NUM])
{
	this->x = x;
	this->y = y;
	this->expireTime = expireTime;
	this->price = price;
	memcpy(this->distanceToHarbors, distanceToHarbors, sizeof(int) * HARBOR_NUM);
}

int Product::getNearestHarborId()
{
	int minDistance = 1000000;
	int minDistanceId = -1;
	for (int i = 0; i < HARBOR_NUM; i++)
	{
		if (distanceToHarbors[i] == -1)
			continue;
		if (distanceToHarbors[i] < minDistance)
		{
			minDistance = distanceToHarbors[i];
			minDistanceId = i;
		}
	}
	return minDistanceId;
}

