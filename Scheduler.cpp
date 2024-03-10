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
		cerr << i << endl;
	}
	// 10行港口
	int x, y, id, time, velocity;
	Coord harborsCoord[10];
	for (int i = 0; i < 10; i++) {
		scanf("%d %d %d %d %d", &id, &x, &y, &time, &velocity);
		harborsCoord[i] = Coord(x, y);
		harbor[i] = Harbor(id, x, y, time, velocity);
	}
	int capacity;
	scanf("%d", &capacity);
	for (int i = 0; i < 5; i++) {
		boat[i] = Boat(i, capacity);
	}

	cerr << "init read over" << endl;
	//初始化路径规划器
	pathPlanner.initHarborPath(map.point,harborsCoord);
	cerr << "init path planner over" << endl;

	// 结尾
	char end[100];
	scanf("%s",end);
	printf("OK\n");
	fflush(stdout);
}

bool Scheduler::NextFrame() {
	cerr << "read frame start" << endl;
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

	cerr<<"read frame over"<<endl;
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
	else
		cerr<< robotId <<" has no harbor"<<endl;
}

void Scheduler::findProductAndHarbor(int robotId)
{
	int startHarbor = robot[robotId].atHarbor; // 将此前设置的目标港口作为出发港口（实际上可能需要再次检测）
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
	else
		cerr<< robotId <<" has no product and harbor"<<endl;
	

}

void Scheduler::Update() {
	// 此处做决策，并输出指令
	cerr << "Update frame: "<< frame<<endl;
	for (int i = 0; i <ROBOT_NUM; i++) 
	{
		if (robot[i].target == -1) 
		{
			cerr << i << " finding product" << endl;
			findProductAndHarbor(i); //为第i个机器人分配任务
		}
		else if (robot[i].target == 1)
		{
			cerr << i << " finding harbor" << endl;
			findHarbor(i); // 第i个机器人去最近的港口 

		}
	}
	//发出指令控制机器人移动
	for (int i = 0; i < ROBOT_NUM; i++)
	{
		int action = robot[i].moveOneStep();
		if (action == 1) //放下物品
			harbor[robot[i].atHarbor].productPrices.push_back(products[robot[i].carryProduct].price);

	}

	// 结尾
	printf("OK\n");
	fflush(stdout);
}