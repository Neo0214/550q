#include "Scheduler.h"

Scheduler::Scheduler() {
	map = Map();
	int robotCount = 0;
	for (int i = 0; i < 200; i++) {
		char line[201] = {0};
		scanf("%s", line);
		// TODO: 此处改为switch case可以加快速度
		for (int j = 0; j < 200; j++) {
			if (line[j] == '.') {
				map.setPoint(i, j, EMPTY);
			}
			else if (line[j] == '*') {
				map.setPoint(i, j, SEA);
			}
			else if (line[j] == '#') {
				map.setPoint(i, j, WALL);
			}
			else if (line[j] == 'A') {
				map.setPoint(i, j, EMPTY); // 直接按空地处理
				this->robot[robotCount]=Robot(robotCount,i,j);
				robotCount++;
			}
			else if (line[j] == 'B') {
				map.setPoint(i, j, HARBOR);
			}
		}
	}
	// 10行港口
	int x, y, id, time, velocity;
	Coord harborsCoord[10];
	for (int i = 0; i < 10; i++) {
		scanf("%d %d %d %d %d", &id, &x, &y, &time, &velocity);
		harborsCoord[i] = Coord(x, y);
		harbor[i] = Harbor(id, x, y, time, velocity);
	}
	// 对港口做初始化处理
	//initHarbor();
	//
	int capacity;
	scanf("%d", &capacity);
	for (int i = 0; i < 5; i++) {
		boat[i] = Boat(i, capacity);
	}
	this->boatCapacity = capacity;
	//初始化路径规划器
	pathPlanner.initHarborPath(map.point,harborsCoord);

	// 结尾
	char end[100];
	scanf("%s",end);
	printf("OK\n");
	fflush(stdout);
}

bool Scheduler::NextFrame() {
	if (frame == 14999) {
		printValue();
	}
	int money;
	if (!scanf("%d %d", &this->frame, &money)) // 读掉第一行frame和money
		return false;
	int newGoodsCount;
	scanf("%d", &newGoodsCount);
	for (int i = 0; i < newGoodsCount; i++) {
		int x, y, val;
		scanf("%d %d %d", &x, &y, &val);
		// map.setGoods(x, y, val, frame + 1000);
		int distanceToHarbors[HARBOR_NUM];
		for (int i = 0; i < HARBOR_NUM; i++)
		{
			distanceToHarbors[i]=pathPlanner.getDistanceToHarbor(i,Coord(x,y)); 
		}
		products.push_back(Product(x, y, val, frame + 1000, distanceToHarbors));
	}
	for (int i = 0; i < 10; i++) {
		int hasGoods, x, y, status;
		scanf("%d %d %d %d", &hasGoods, &x, &y, &status);
		if (status == OFF)
		{
			cerr << "collide!" << endl;
		}
		robot[i].update(hasGoods, x, y, status);
	}
	for (int i = 0; i < 5; i++) {
		int status, pos;
		scanf("%d %d", &status, &pos);
		boat[i].update(status, pos);
	}
	char end[100];
	scanf("%s", end); // 读掉结尾
	
	
	return true;
}

void Scheduler::findHarbor(int robotId)
{
	int minDistance = 10000;
	int bestHarborId = -1;
	for (int i = 0; i < HARBOR_NUM; i++)
	{
		int distance = pathPlanner.getDistanceToHarbor(i,Coord(robot[robotId].x,robot[robotId].y));
		if (distance!=-1 && distance < minDistance)
		{
			minDistance = distance;
			bestHarborId = i;
		}

	}
	if (bestHarborId != -1)
	{
		robot[robotId].assignTask(pathPlanner.getPathToHarbor(bestHarborId, Coord(robot[robotId].x, robot[robotId].y)), 2, bestHarborId);
	}

}

void Scheduler::findProductAndHarbor(int robotId)
{
	int startHarbor = robot[robotId].atHarbor; // 将此前findHarbor设置的目标港口作为出发港口
	if (startHarbor == -1)
	{
		cerr << "error origin harbor" << endl;
		return;
	}


	int bestProductId;
	int bestHarborId = -1;
	double maxProfitRate = -1;
	for (int i=0;i<products.size();i++)
	{
		if(products[i].locked) // 产品被锁定
			continue;
		int nearestHarborId = products[i].getNearestHarborId();
		if (nearestHarborId == -1) // 产品没有可达港口
			continue;
		int pathLen1 = products[i].distanceToHarbors[startHarbor]; 
		if (pathLen1 == -1) // 产品不可到达机器人出发港口
			continue;
		if(frame+pathLen1>=products[i].expireTime) // 机器人到达产品的时间超过了过期时间
			continue;
		int pathLen2 = products[i].distanceToHarbors[nearestHarborId];
		int pathLen = pathLen1 + pathLen2;
		double profitRate = pow(products[i].price,1 + 0.1 * robotId) / pathLen * exp(-0.00001*(products[i].expireTime-frame)) + 1.0 / ((pathLen1 - pathLen2)*(pathLen1 - pathLen2) + 0.5);
		if(profitRate > maxProfitRate)
		{
			maxProfitRate=profitRate;
			bestProductId=i;
			bestHarborId=nearestHarborId;
		}
	}

	if (bestHarborId != -1)
	{
		robot[robotId].assignTask(pathPlanner.getPathFromHarbor(startHarbor, Coord(products[bestProductId].x, products[bestProductId].y)), 0, -1);
		products[bestProductId].locked = true;
		robot[robotId].carryProduct = bestProductId;
	}



}

void Scheduler::initHarbor() {
	// 对速度排序
	int vel[10] = { 0 };
	for (int i = 0; i < HARBOR_NUM; i++) {
		vel[i] = harbor[i].velocity;
		harborVelIndex[i] = i;
	}
	for (int i = 0; i < HARBOR_NUM; i++) {
		for (int j = 0; j < HARBOR_NUM - i; j++) {
			if (vel[j] < vel[j + 1]) {
				int tmp = vel[j];
				vel[j] = vel[j + 1];
				vel[j + 1] = tmp;

				int tmpIndex = harborVelIndex[j];
				harborVelIndex[j] = harborVelIndex[j + 1];
				harborVelIndex[j + 1] = tmpIndex;
			}
		}
	}
}

void Scheduler::Update() {

	// 此处做决策，并输出指令
	for (int i = 0; i <ROBOT_NUM; i++) 
	{
		if (robot[i].target == -1) 
		{

			findProductAndHarbor(i); //为第i个机器人分配任务
		}
		else if (robot[i].target == 1)
		{

			findHarbor(i); // 第i个机器人去最近的港口 

		}
	}
	//发出指令控制机器人移动

	// 碰撞图初始化
	int collisionMap[LEN][LEN] = { 0 };
	for (int i = 0; i < LEN; i++)
	{
		for (int j = 0; j < LEN; j++)
		{
			switch (map.point[i][j])
			{
				case EMPTY:
				case HARBOR:
					collisionMap[i][j] = 0;
					break;
				default:
					collisionMap[i][j] = -1;
					break;
			}
		}
	}
	for (int i = 0; i < ROBOT_NUM; i++)
	{
		collisionMap[robot[i].x][robot[i].y] = i + 1; // 注意这里是i+1，因为机器人id从0开始
	}

	// 移动栈和剩余栈
	stack<pair<int, int>> moveRobots;
	stack<pair<int, int>> restRobots;
	// 剩余栈初始化
	for (int i = ROBOT_NUM - 1; i >= 0; i--) 
	{
		restRobots.push(make_pair(i,-1)); // first是机器人id，second是机器人的移动方向
	}

	// 找到每个机器人的无冲突移动方向
	while(!restRobots.empty())
	{
		pair<int,int> curRobot = restRobots.top();
		restRobots.pop();
		
		pair<int, vector<int>> moveAndCollision = robot[curRobot.first].moveOneStep(collisionMap);

		while (moveAndCollision.second.size())
		{
			cerr << "tough task" << moveAndCollision.second.size() << endl;
			pair<int,int> redoRobot = moveRobots.top();
			moveRobots.pop();
			robot[redoRobot.first].redoOneStep(collisionMap,redoRobot.second);
			auto iter = find(moveAndCollision.second.begin(), moveAndCollision.second.end(), redoRobot.first);
			if (iter != moveAndCollision.second.end())
				moveAndCollision.second.erase(iter);
			restRobots.push(redoRobot);
		}
		curRobot.second = moveAndCollision.first;
		moveRobots.push(curRobot);
	}

	//发出指令控制机器人移动
	while(!moveRobots.empty())
	{
		pair<int, int> curRobot = moveRobots.top();
		moveRobots.pop();
		int i= curRobot.first;
		int action = robot[i].command(curRobot.second);
		if (action == 1) //放下物品
		{
			harbor[robot[i].atHarbor].productPrices.push_back(products[robot[i].carryProduct].price);
			score += products[robot[i].carryProduct].price;
		}
		
	}



	// 放下物品以后，才能装上船
		// 单独处理货物搬上船的问题
		// 如果在港口，将货物搬上船
	for (int i = 0; i < 5; i++)
	{
		if (boat[i].getPos() != -1 && boat[i].getStatus() == NORMAL) {


			loadGoods(&boat[i], &harbor[boat[i].getPos()]);
		}
	}


	// 以上是机器人的移动，以下是船的移动
	// 首先看船是否需要回去交货
	for (int i = 0; i < 5; i++) {
		if (boat[i].getCurCapacity() == boatCapacity
			|| (boat[i].getPos() != -1 && boat[i].getStatus() == NORMAL && 14999 - frame < harbor[boat[i].getPos()].time)) {
			boat[i].comeBack();
			harbor[boat[i].getPos()].isLocked = false;
		}
	}
	// 然后看船是否需要换一个港口
	for (int i = 0; i < 5; i++) {

		if (boat[i].getPos() != -1 && boat[i].getStatus() == NORMAL && boat[i].getPre() == 0 && harbor[boat[i].getPos()].productPrices.size()==0) {
			// 此时船在港口，但是没有货物上船，考虑更换港口
			int harborId = selectFastestHarbor(boatCapacity-boat[i].getCurCapacity());
			if (harborId != -1) {
				harbor[boat[i].getPos()].isLocked = false;

				harbor[harborId].isLocked = true;
				boat[i].gotoHarbor(harborId);
			}
			// 如果是-1，说明没有合适的港口，立刻回去交货
			else {
				boat[i].comeBack();
				harbor[boat[i].getPos()].isLocked = false;
			}
		}
	}
	vector<int> freeBoat = getFreeBoat();
	// 根据空闲数量挑选货最多的港口
	for (int i = 0; i < freeBoat.size(); i++) {
		int harborId = selectAvailableFastestHarborWithGoingFromOriginPoint();
		// 要注意此处boat从交货地点出发，要考虑是否还有时间回来交货
		if (harborId != -1) {
			harbor[harborId].isLocked = true;
			boat[freeBoat[i]].gotoHarbor(harborId);
		}
	}
	
	
	// 结尾
	printf("OK\n");
	fflush(stdout);
}

int Scheduler::selectAvailableFastestHarborWithGoingFromOriginPoint() {
	int bestID = -1;
	int value = -1;
	for (int i = 0; i < 10; i++) {
		if (harbor[i].isLocked || harbor[i].time+harbor[i].time+frame>=15000) {
			continue;
		}
		else {
			int tmpValue=getFutureValue(i,boatCapacity);
			if (tmpValue > value) {
				value = tmpValue;
				bestID = i;
			}
		}
	}
	return bestID;
}

int Scheduler::selectFastestHarbor(int countNeeded) {
	int bestID = -1;
	int value = -1;
	for (int i = 0; i < 10; i++) {
		if (harbor[i].isLocked) {
			continue;
		}
		else {
			int tmpValue = getFutureValue(i,countNeeded);
			if (tmpValue > value) {
				value = tmpValue;
				bestID = i;
			}
			
		}
	}
	return bestID;
}

int Scheduler::getFutureValue(int harborId, int total) {
	int count = 0;
	int i = 0;
	for (; i < harbor[harborId].productPrices.size() && i<total; i++) {
		count += harbor[harborId].productPrices[i];
	}
	// 在此处判断时间是否还够换港口
	// i+1 才是真正的货物数量
	int loadcost = (i + 1) / harbor[harborId].velocity;
	if (loadcost + harbor[harborId].time + 500 + frame >= 15000) {
		return -1;
	}
	return count;
}

int Scheduler::getValue(int harborId) {
	int count = 0;
	for (int i = 0; i < harbor[harborId].productPrices.size(); i++) {
		count += harbor[harborId].productPrices[i];
	}
	return count;
}

vector<int> Scheduler::getFreeBoat() {
	vector<int> boatIds;
	for (int i = 0; i < 5; i++) {
		if (boat[i].getPos() == -1 && boat[i].getStatus()==NORMAL)
			boatIds.push_back(i);
	}
	return boatIds;
}

void Scheduler::loadGoods(Boat* _boat, Harbor* _harbor)
{
	int num = min(_harbor->velocity, int(_harbor->productPrices.size()));
	_boat->addGoods(num);

	_harbor->productPrices.erase(_harbor->productPrices.begin(), _harbor->productPrices.begin() + num);

}

void Scheduler::printValue() {
	for (int i = 0; i < HARBOR_NUM; i++) {
		int count = 0;
		for (int j = 0; j < harbor[i].productPrices.size(); j++) {
			count+=harbor[i].productPrices[j];
		}
		cerr<<"harbor "<<i<<" "<<count<<endl;
	}
	cerr <<"final " << score << endl;
}