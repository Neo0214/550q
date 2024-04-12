#pragma once



#include "defines.h"
#include "Order.h"
#include "Coord.h"
class BoatPathPlanner;
#define MOVING 0
#define RECOVER 1
#define LOADING 2

class Boat {
	friend class Scheduler;
private:
	// 固定数据
	int id;
	int capacity;
	// 动态数据
	int status;
	Coord pos;
	Coord prePos;
	int direction;
	int curCapacity;
	int curValue;
	int preLoadNum;
	vector<Order> orders;
	int turns;
	bool atLast = false;
	vector<int> action;
	int curAct;
	bool force;
	vector<int> target; // 港口id---------售卖id
	int preTarget;
	bool isFirst = true;
	int viaIndex;
	// 私有函数

public:
	Boat(int _id, int _capacity);
	Boat();
	// 公共函数
	void update(int status, int curCapacity, int x, int y, int direction);
	int getStatus();
	int getPos();
	int getCurCapacity();
	void addGoods(int num, int value);
	int getCurValue();
	int getPre();
	void clearOrders();
	void clearOneOrder();
	void addOneOrder();
	int getOrderCapacity();
	void newOrder(int goodsLeft, int harborId);
	int originPos();
	void act(int act);
	void leave();
	void driveIn();
	void rot(int rotDirect);
	void forward();
	bool isFree();
	void nextAct(int harborNum, BoatPathPlanner* boatPathPlanner);
};
