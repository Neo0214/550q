#include "Harbor.h"

Harbor::Harbor(int _Id,int _x, int _y, int _time, int _velocity)
{
	Id = _Id;
	leftTopX = _x;
	leftTopY = _y;
	time = _time;
	velocity = _velocity;
	orders=vector<Order>();
	area = 0;
}
Harbor::Harbor() {

}


void Harbor::getBestCoord(int points[LEN][LEN])
{
	// 从左上角开始遍历
	Coord searchCoords[12] = { Coord(leftTopX+1, leftTopY),Coord(leftTopX + 2, leftTopY),Coord(leftTopX + 1, leftTopY+3),Coord(leftTopX + 2, leftTopY+3),
		Coord(leftTopX, leftTopY + 1),Coord(leftTopX, leftTopY + 2),Coord(leftTopX+3, leftTopY + 1),Coord(leftTopX+3, leftTopY + 2),
		Coord(leftTopX,leftTopY),Coord(leftTopX + 3, leftTopY),Coord(leftTopX, leftTopY + 3),Coord(leftTopX + 3, leftTopY + 3)
	};
	// 右左上下 0123
	int searchDirection[16] = { 1,1,0,0,2,2,3,3,1,1,0,0,2,3,2,3 };
	for (int i = 0; i < 12; i++)
	{
		Coord searchCoord = searchCoords[i];
		Coord neighborCoord = searchCoords[i] + searchDirection[i];
		if (points[neighborCoord.x][neighborCoord.y] == EMPTY)
		{
			this->x = searchCoord.x;
			this->y = searchCoord.y;
			return;
		}
	}
	for (int i = 8; i < 12; i++)
	{
		Coord searchCoord = searchCoords[i];
		Coord neighborCoord = searchCoords[i] + searchDirection[i+4];
		if (points[neighborCoord.x][neighborCoord.y] == EMPTY)
		{
			this->x = searchCoord.x;
			this->y = searchCoord.y;
			return;
		}
	}
}

void Harbor::clearOneOrder() {
	this->orders.erase(this->orders.begin(), this->orders.begin() + 1);
	//this->endingCost+=0.5;
}

void Harbor::addOneOrder() {
	this->orders[0].add();
}

void Harbor::newOrder(int goodsLeft, int boatId) {
	Order order(boatId, goodsLeft);
	orders.push_back(order);
	//this->endingCost-=0.5;
}

double Harbor::getExpectedProfitRate(int robotId, double exceptProfit)
{
	removeProfitRate(exceptProfit); // 先删除
	int idex = find(robotsGoalHarbor.begin(), robotsGoalHarbor.end(), robotId) - robotsGoalHarbor.begin();
	double sum = accumulate(
		expectedProfitRate.begin() + std::min(productPerAgent * idex,int(expectedProfitRate.size())),
		expectedProfitRate.begin() + std::min(productPerAgent * (idex + 1), int(expectedProfitRate.size())),
		0.0);
	appendProfitRate(exceptProfit); // 再添加
	return sum;
}

void Harbor::appendRobot(int robotId)
{
	robotsGoalHarbor.push_back(robotId);
	return;
}

void Harbor::removeRobot(int robotId)
{
	robotsGoalHarbor.erase(find(robotsGoalHarbor.begin(), robotsGoalHarbor.end(), robotId));
	return;
}

void Harbor::appendProfitRate(double profitRate)
{
	auto iter = lower_bound(expectedProfitRate.begin(), expectedProfitRate.end(), profitRate, [](double a, double b) {
		return a > b;
		}); // 二分查找
	expectedProfitRate.insert(iter, profitRate);
	return;
}

void Harbor::removeProfitRate(double profitRate)
{
	expectedProfitRate.erase(find(expectedProfitRate.begin(), expectedProfitRate.end(), profitRate));
	return;
}





