#pragma once
#include "defines.h"
#include "Map.h"
#include "Harbor.h"
#include "Boat.h"
#include "Robot.h"
#include "Product.h"
#include "PathPlanner.h"
#include "args.h"
#include "BoatPathPlanner.h"
#include "Coord.h"
#include "Delivery.h"

class Scheduler {
private:
	// 固定数据
	Map map;
	vector<Harbor> harbors;
	vector<Boat> boats;
	vector<Robot> robots;
	vector<Product> products;
	int harborNum; // 港口数量
	vector<Coord> robotBuyPlace; // 购买机器人处
	vector<Coord> boatBuyPlace; // 购买船处
	vector<Delivery> boatDeliveryPlace; // 交货处
	int robotNum; // 机器人数量
	PathPlanner pathPlanner; // 路径规划器
	BoatPathPlanner boatPathPlanner; // 船的路径规划器
	int boatNum; // 船的数量
	int boatCapacity; // 船的容量
	int minBackTime; // 最小回港时间
	// 动态数据
	int frame; // 当前帧号
	int score = 0;
	int startProductId = 0;
	vector<int> harborWhoGotReceive; // 记录哪个港口在这一帧中接收了货物

	// 私有函数
	void findHarbor(int robotId); //找最近的港口
	void findProductAndHarbor(int robotId); //找收益率最高的物品和对应港口
	void loadGoods(Boat* _boat, Harbor* _harbor);
	void clearWhenBoatComeBack(int boatId, int harborId);
	void clearWhenBoatSwitch(int boatId, int harborId);
	void synchronizeWhenGoOut(int boatId, int harborId);
	void synchronizeWhenSwitch(int boatId, int harborId);
	void buyBoat(int buyIndex);
	void buyRobot(int buyIndex);
	bool boatAtBuy(int boatId);
	void setBestBerthCoord(Harbor& curHarbor, char my_map[LEN][LEN]);
	int findBestHarbor(const Coord& pos);
	bool atTarget(Coord pos, int targetId);
public:
	Scheduler();

	bool NextFrame();
	void Update();
	void printValue();
	void printDebug();
};