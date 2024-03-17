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
	if (frame == 15000) {
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
		robot[robotId].assignTask(pathPlanner.getPathToHarbor(bestHarborId, Coord(robot[robotId].x, robot[robotId].y)), 2);
		robot[robotId].atHarbor = bestHarborId; // 设置目标港口
	}

}

void Scheduler::findProductAndHarbor(int robotId)
{
	int startHarbor = robot[robotId].atHarbor; // 将此前设置的目标港口作为出发港口（实际上可能需要再次检测）
	if (startHarbor == -1)
	{
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
		robot[robotId].assignTask(pathPlanner.getPathFromHarbor(startHarbor, Coord(products[bestProductId].x, products[bestProductId].y)), 0);
		products[bestProductId].locked = true;
		robot[robotId].carryProduct = bestProductId;
		robot[robotId].atHarbor = -1;
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
		collisionMap[robot[i].x][robot[i].y] = i + 1;
	}

	int moveOfRobots[10];
	for (int i = 0; i < ROBOT_NUM; i++)
	{
		pair<int, int> moveAndCollision;
		//cerr << '(' << robot[i].x << ',' << robot[i].y << ')';
		moveAndCollision = robot[i].moveOneStep(collisionMap);
		moveOfRobots[i] = moveAndCollision.first;
		Coord newCoord = Coord(robot[i].x, robot[i].y) + moveOfRobots[i];
		//cerr << '(' << newCoord.x << ',' << newCoord.y << ')';
		//cerr << endl;
		if (moveAndCollision.second != -1)
		{
			cerr<<"Not implement"<<endl;
		}
	}

	//发出指令控制机器人移动
	for (int i = 0; i < ROBOT_NUM; i++)
	{
		int action = robot[i].command(moveOfRobots[i]);
		if (action == 1) //放下物品
		{
			harbor[robot[i].atHarbor].productPrices.push_back(products[robot[i].carryProduct].price);
			score += products[robot[i].carryProduct].price;
		}
		
	}



	


	// 以上是机器人的移动，以下是船的移动
	// 首先看船是否需要回去交货
	for (int i = 0; i < 5; i++) {
		// 当恰好装满或者时间不够时，立刻回去交货
		if (boat[i].getCurCapacity() == boatCapacity
			|| (boat[i].getPos() != -1 && boat[i].getStatus() == NORMAL && 15000 - frame < harbor[boat[i].getPos()].time)
			) {
			boat[i].comeBack();
			harbor[boat[i].getPos()].leftGoodsNumber = 0;
		}
	}
	// 然后看船是否需要换一个港口
	for (int i = 0; i < 5; i++) {
		// 当船停靠在某个港口，且该港口恰好没有堆积的货物可以给装载，寻找可换的港口（此处需要考虑换港口是否划算，不要去太少堆积的地方）
		if (boat[i].getPos() != -1 && boat[i].getStatus() == NORMAL && boat[i].getPre() == 0 && harbor[boat[i].getPos()].productPrices.size()==0) {
			int harborId = selectFastestHarbor(boatCapacity-boat[i].getCurCapacity(),harbor[boat[i].getPos()].time,boat[i].getCurValue());
			// 若有可换的，立刻过去
			if (harborId != -1) {
				harbor[boat[i].getPos()].leftGoodsNumber = 0;
				//harbor[harborId].isLocked = true;
				boat[i].gotoHarbor(harborId);
			}
			// 如果是-1，说明没有合适的港口，立刻回去交货
			else {
				boat[i].comeBack();
				harbor[boat[i].getPos()].leftGoodsNumber = 0;
			}
		}
	}
	vector<int> freeBoat = getFreeBoat();
	// 根据空闲数量挑选货最多的港口
	for (int i = 0; i < freeBoat.size(); i++) {
		int harborId = selectAvailableFastestHarborWithGoingFromOriginPoint();
		// 要注意此处boat从交货地点出发，要考虑是否还有时间回来交货
		if (harborId != -1) {
			//harbor[harborId].isLocked = true;
			boat[freeBoat[i]].gotoHarbor(harborId);
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

		



	// 结尾
	printf("OK\n");
	fflush(stdout);
}

int Scheduler::getFutureValueFromOriginPoint(int harborId, int total) {
	int count = 0;
	int i = 0;
	for (; i < harbor[harborId].productPrices.size() && i<total; i++) {
		count += harbor[harborId].productPrices[i];
	}
	// count为可在港口装载的货物总价值
	// 在此处判断时间是否还够换港口
	// i+1 才是真正的货物数量
	int loadcost = (i + 1) / harbor[harborId].velocity;
	if (loadcost + harbor[harborId].time + 500 + frame >= 15000) {
		return -1;
	}
	return count;
}

int Scheduler::selectAvailableFastestHarborWithGoingFromOriginPoint() {
	int bestID = -1;
	float value = -1;
	for (int i = 0; i < 10; i++) {
		if (harbor[i].time+harbor[i].time+frame>=15000) {
			continue;
		}
		else {
			float tmpValue=getFutureValueFromOriginPoint(i,boatCapacity);
			if (tmpValue > value) {
				value = tmpValue;
				bestID = i;
			}
		}
	}
	return bestID;
}
/*
* @param countNeeded 还能装载的货物数量
* @param timeToComeBack 直接回去交货的时间
* @param holdedValue 已装的货物价值
*/
int Scheduler::selectFastestHarbor(int countNeeded, int timeTocomeBack, int holdedValue) {
	int bestID = -1;
	float value = -1;
	for (int i = 0; i < 10; i++) {
		// 计算未来的价值（单位：效率 钱/时间）
		float tmpValue = getFutureValue(i, countNeeded);
		// (处必须考虑，若该港口货物非常多，是否要排队，排队可能也是划算的)

		if (tmpValue > value) {
			value = tmpValue;
			bestID = i;
		}
	}
	if (countNeeded>10 || (bestID != -1 && value>holdedValue/float(timeTocomeBack*2))) {
		return bestID;
	}
	return -1;
}
/*
* @param harborId 目标港口id
* @param total    最大可装货物数量
*/
float Scheduler::getFutureValue(int harborId, int total) {
	int count = 0;
	int i = harbor[harborId].leftGoodsNumber;
	int number = 0;
	for (; i < harbor[harborId].productPrices.size() && number<total; i++) {
		count += harbor[harborId].productPrices[i];
		number++;
	}
	// count为可在港口装载的货物总价值
	// 在此处判断时间是否还够换港口
	int loadcost = number / harbor[harborId].velocity;
	if (loadcost + harbor[harborId].time + 500 + frame >= 15000) {
		return -1;
	}
	return count/float(loadcost+500);
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
	int num = min(min(_harbor->velocity, int(_harbor->productPrices.size())),boatCapacity-_boat->getCurCapacity());
	int value = 0;
	for (int i = 0; i < num; i++) {
		value+=_harbor->productPrices[i];
	}
	_boat->addGoods(num,value);
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