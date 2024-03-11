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
#ifdef DEBUG
	else
		cerr << robotId << " no harbor" << endl;
#endif
}

void Scheduler::findProductAndHarbor(int robotId)
{
	int startHarbor = robot[robotId].atHarbor; // 将此前设置的目标港口作为出发港口（实际上可能需要再次检测）
	if (startHarbor == -1)
	{
#ifdef DEBUG
		cerr << robotId << " is not at harbor" << endl;
#endif
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
		if(frame+pathLen1>=products[i].expireTime) // 机器人到达产品的时间超过了过期时间
			continue;

		int pathLen = pathLen1 + products[i].distanceToHarbors[nearestHarborId];
		double profitRate = (double)products[i].price / pathLen;
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
	}
#ifdef DEBUG
	else
		cerr << robotId << " no product" << endl;
#endif


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
#ifdef DEBUG
			cerr << i << " findProduct" << endl;
#endif
			findProductAndHarbor(i); //为第i个机器人分配任务
		}
		else if (robot[i].target == 1)
		{
#ifdef DEBUG
			cerr << i << " findHarbor" << endl;
#endif
			findHarbor(i); // 第i个机器人去最近的港口 

		}
	}
	//发出指令控制机器人移动
	for (int i = 0; i < ROBOT_NUM; i++)
	{
		int action = robot[i].moveOneStep();
		if (action == 1) //放下物品
			harbor[robot[i].atHarbor].productPrices.push_back(products[robot[i].carryProduct].price);
		// 放下物品以后，才能装上船
		// 单独处理货物搬上船的问题
		// 如果在港口，将货物搬上船
		for (int i = 0; i < 1; i++)
		{
			if (boat[i].getPos() != -1 && boat[i].getStatus()==NORMAL && i==0) {
				//cerr<<"boat 0 has "<<boat[0].getCurCapacity() << endl;
				//cerr <<"harbor 1 has"<< harbor[boat[0].getPos()].productPrices.size() << endl;
				//cerr << "boat 0:" << &boat[i] << endl;
				//cerr << "harbor 1:" << &harbor[boat[i].getPos()] << endl;
				loadGoods(&boat[i], &harbor[boat[i].getPos()]);
			}
		}
	}
	// 以上是机器人的移动，以下是船的移动
	if (frame > 500) {
		for (int i = 0; i < 5; i++) {
			if (boat[i].getCurCapacity() == boatCapacity || (boat[i].getPos()!=-1 && boat[i].getStatus()==NORMAL && 14999-frame<harbor[boat[i].getPos()].time)) {
				boat[i].comeBack();
				harbor[boat[i].getPos()].isLocked = false;
			}
		}
		// 过一会再开始猜测
		int blockVel[10] = { 0 };
		for (int i = 0; i < HARBOR_NUM; i++) {
			int blockNum = int(harbor[i].productPrices.size()); // 已堆积的数量
			blockVel[i] = float(blockNum) / frame; // 堆积速度
		}
		vector<int> freeBoat = getFreeBoat();
		// 根据空闲数量挑选速率最快的港口
		for (int i = 0; i < freeBoat.size(); i++) {
			int harborId = selectFastestHarbor(blockVel); 
			// 一定能挑选出一个
			harbor[harborId].isLocked = true;
			boat[freeBoat[i]].gotoHarbor(harborId);
		}
	}
	
	
	// 结尾
	printf("OK\n");
	fflush(stdout);
}

int Scheduler::selectFastestHarbor(int* blockVel) {
	int maxID = -1;
	int maxLoadVel = -1;
	float maxVel = -1;
	for (int i = 0; i < 10; i++) {
		if (harbor[i].isLocked) {
			continue;
		}
		else {
			if (harbor[i].velocity > maxLoadVel) {
				if (harbor[i].productPrices.size() / float(frame) > maxVel) {
					maxID = i;
					maxLoadVel=harbor[i].velocity;
					maxVel = harbor[i].productPrices.size() / frame;
				}
			}
		}
	}
	return maxID;
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
	//cerr<<_harbor->productPrices.size()<<endl;
	_harbor->productPrices.erase(_harbor->productPrices.begin(), _harbor->productPrices.begin() + num);
	//cerr<<harbor->productPrices.size()<<endl;
}