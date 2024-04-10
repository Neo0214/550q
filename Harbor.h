#pragma once
#include "defines.h"
#include "Order.h"
#include "Boat.h"
#include "args.h"
#include "Coord.h"

class Harbor {
public:
	// 固定数据
	int Id;
	Coord robotCoord; // 港口中的机器人目标坐标
	Coord boatCoord; // 港口中（靠泊）的船核心点坐标
	vector<Coord> circleCoord; // 外圈坐标
	int velocity;


	//float area;
public:
	// 动态数据
	vector<int> productPrices;
	vector<Order> orders; // 记录港口已经被分配的订单
	float endingCost = 1; // 快结束时，是否被选中
	int totalReceived = 1;
	bool tiger = false;
	double prioTiger = 1;

	vector<double> expectedProfitRate; // 港口附近的物品的收益率（从大到小）
	vector<int> robotsGoalHarbor; // 负责该港口的机器人
	vector<Coord> getCoord();

public:
	Harbor(int _Id, int _x, int _y, int _velocity);
	Harbor();
	// 公共函数
	void getBestCoord(char points[LEN][LEN]);
	void clearOneOrder();
	void addOneOrder();
	void newOrder(int goodsLeft, int boatId);
	double getExpectedProfitRate(int robotId, double exceptProfit);
	void appendRobot(int robotId);
	void removeRobot(int robotId);
	void appendProfitRate(double profitRate);
	void removeProfitRate(double profitRate);
};