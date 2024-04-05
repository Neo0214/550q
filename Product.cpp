#include "Product.h"

Product::Product(int x, int y, int price, int expireTime, const vector<int>& distanceToHarbors)
{
	this->x = x;
	this->y = y;
	this->expireTime = expireTime;
	this->price = price;
	this->distanceToHarbors = distanceToHarbors;

	int minDistance = INT_MAX;
	int minDistanceId = -1;
	for (int i = 0; i < distanceToHarbors.size(); i++)
	{
		if (distanceToHarbors[i] == -1)
			continue;
		if (distanceToHarbors[i] < minDistance)
		{
			minDistance = distanceToHarbors[i];
			minDistanceId = i;
		}
	}
	this->nearestHarborId = minDistanceId;
	this->bestProfitRate = double(price) / minDistance;

}

int Product::getNearestHarborId()
{
	return nearestHarborId;
}



