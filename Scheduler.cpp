#include "Scheduler.h"

Scheduler::Scheduler() {
	map = Map();
	int robotCount = 0;
	for (int i = 0; i < 200; i++) {
		char line[201] = {0};
		scanf("%s", line);
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
	cerr << "init  over" << endl;


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
	if(bestHarborId!=-1)
		robot[robotId].assignTask(pathPlanner.getPathToHarbor(bestHarborId, Coord(robot[robotId].x, robot[robotId].y)), bestHarborId);
	else
		cerr<< robotId <<" has no harbor"<<endl;
}

void Scheduler::findProductAndHarbor(int robotId)
{
	int startHarbor = robot[robotId].atHarbor;
	Product bestProduct;
	int bestHarborId = -1;
	double maxProfitRate = -1;
	for (Product product : products)
	{
		//TODO 检查产品是否被锁定和是否会过期......

		int nearestHarborId = product.getNearestHarborId();
		int pathLen = product.distanceToHarbors[startHarbor] + product.distanceToHarbors[nearestHarborId];
		double profitRate = (double)product.price / pathLen;
		if(profitRate > maxProfitRate)
		{
			maxProfitRate=profitRate;
			bestProduct=product;
			bestHarborId=nearestHarborId;
		}
		robot[robotId].assignTask(pathPlanner.getPathFromHarbor(startHarbor,Coord(bestProduct.x,bestProduct.y)), bestHarborId);
	}
	

}

void Scheduler::Update() {
	// 此处做决策，并输出指令
	cerr<<"Update frame:"<<frame<<endl;
	for (int i = 0; i < 10; i++) 
	{
		if (robot[i].target == -1) 
		{
			// 机器人没有目标，需要分配
			if (robot[i].atHarbor == -1)
			{
				// TODO: 拿着货物去港口
				cerr<<i<<"find harbor"<<endl;
				findHarbor(i); // 第i个机器人去最近的港口 
			}
			else
			{
				findProductAndHarbor(i); //为第i个机器人分配任务
			}
		}
	}
	//发出指令控制机器人移动
	for (int i = 0; i < 10; i++)
	{
		robot[i].moveOneStep();
	}

	// 结尾
	printf("OK\n");
	fflush(stdout);
}