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
	}
	// 10行港口
	int x, y, id, time, velocity;
	Coord harborsCoord[10];
	for (int i = 0; i < 10; i++) {
		scanf("%d %d %d %d %d", &id, &x, &y, &time, &velocity);
		harbor[i] = Harbor(id, x, y, time, velocity);
		harbor[i].getBestCoord(map.point);
		harborsCoord[i] = Coord(harbor[i].x,harbor[i].y);
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

	int minTime= INT_MAX;
	for (int i = 0; i < 10; i++) {
		if (harbor[i].time < minTime) {
			minTime = harbor[i].time;
		}
	}
	minBackTime = minTime;

	// 初始化港口覆盖率
	for (int x = 0; x < 200; x++) {
		for (int y = 0; y < 200; y++) {
			vector<int> moreThanOne=vector<int>();
			for (int i = 0; i < 10; i++) {
				if (pathPlanner.getDistanceToHarbor(i, Coord(x, y)) <= 30) {
					moreThanOne.push_back(i);
				}
			}
			if (moreThanOne.size() > 1) {
				for (int i = 0; i < moreThanOne.size(); i++) {
					harbor[moreThanOne[i]].area+=0.000005;
				}
			}
			moreThanOne.clear();
		}
	}

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
		Product newProduct = Product(x, y, val, frame + 1000, distanceToHarbors);
		products.push_back(newProduct);

		int nearestHarborId = newProduct.getNearestHarborId();
		if (nearestHarborId != -1) // 产品有可达港口
			harbor[nearestHarborId].appendProfitRate(newProduct.bestProfitRate);
	}
	for (int i = startProductId; i < products.size(); i++)
	{
		if (products[i].expireTime <= frame)
		{
			startProductId++;
			//看情况是否加上下面的代码
			//int nearestHarborId;
			//if(!products[i].locked && (nearestHarborId = products[i].getNearestHarborId()) != -1)
			//	harbor[nearestHarborId].removeProfitRate(products[i].bestProfitRate);
		}
		else
			break;
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
	float minDistance = 10000;
	int bestHarborId = robot[robotId].goalHarbor;
	if(bestHarborId==-1 || harbor[bestHarborId].tiger) // 如果已经指定港口则不再查找
		for (int i = 0; i < HARBOR_NUM; i++)
		{
			if (harbor[i].tiger)
				continue;
			float distance = pathPlanner.getDistanceToHarbor(i, Coord(robot[robotId].x, robot[robotId].y));
			if (distance >0 && distance < minDistance)
			{
				minDistance = distance;
				bestHarborId = i;
			}

		}
	if (bestHarborId != -1)
	{
		if(robot[robotId].goalHarbor==-1) // 初始情况，原先没有位于港口
			harbor[bestHarborId].appendRobot(robotId);
		else if (robot[robotId].goalHarbor != bestHarborId) // 正常来说不会进入此if，除非在findHarbor中目标港口发生变化
		{
			harbor[robot[robotId].goalHarbor].removeRobot(robotId);
			harbor[bestHarborId].appendRobot(robotId);
		}
		robot[robotId].assignTask(pathPlanner.getPathToHarbor(bestHarborId, Coord(robot[robotId].x, robot[robotId].y)), 2, bestHarborId, bestHarborId);
	}

}

void Scheduler::findProductAndHarbor(int robotId)
{
	if (robot[robotId].moves.size() != robot[robotId].curMoveCount) // 如果还有未走完的路不要设置目标
		return;
	int startHarbor = robot[robotId].atHarbor; // 将此前findHarbor设置的目标港口作为出发港口

	int bestProductId;
	int bestHarborId = -1;
	double maxProfitRate = -1;
	for (int i=startProductId;i<products.size();i++)
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
		double profitRate = (pow(products[i].price, 1 + robotId * valueImportance) / pathLen * exp(-expireTimeImportance * (products[i].expireTime - frame))
			+ harbor[nearestHarborId].getExpectedProfitRate(robotId, products[i].bestProfitRate) / productPerAgent * expectedWeight
			);
		if(profitRate > maxProfitRate)
		{
			maxProfitRate=profitRate;
			bestProductId=i;
			bestHarborId=nearestHarborId;
		}
	}
	if (bestHarborId != -1)
	{
		if (robot[robotId].goalHarbor != bestHarborId) // 机器人目标港口发生变化
		{
			harbor[robot[robotId].goalHarbor].removeRobot(robotId);
			harbor[bestHarborId].appendRobot(robotId);
		}
		robot[robotId].assignTask(pathPlanner.getPathFromHarbor(startHarbor, Coord(products[bestProductId].x, products[bestProductId].y)), 0, -1, bestHarborId);
		products[bestProductId].locked = true;
		harbor[bestHarborId].removeProfitRate(products[bestProductId].bestProfitRate); // 商品锁定后，港口对应收益率减少
		robot[robotId].carryProduct = bestProductId;
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
	int countToughTask = 0;
	while(!restRobots.empty())
	{
		pair<int,int> curRobot = restRobots.top();
		restRobots.pop();
		
		pair<int, vector<int>> moveAndCollision = robot[curRobot.first].moveOneStep(collisionMap);
		if(countToughTask<=10)
			while (moveAndCollision.second.size())
			{
				//cerr << "tough task" << moveAndCollision.second.size() <<' ' << moveRobots.size() <<endl;
				countToughTask++;
				pair<int,int> redoRobot = moveRobots.top();
				moveRobots.pop();
				robot[redoRobot.first].redoOneStep(collisionMap,redoRobot.second);
				auto iter = find(moveAndCollision.second.begin(), moveAndCollision.second.end(), redoRobot.first);
				if (iter != moveAndCollision.second.end())
					moveAndCollision.second.erase(iter);
				restRobots.push(redoRobot);
			}
		else
			while (moveAndCollision.second.size() && !moveRobots.empty())
			{
				//cerr << "really tough" << endl;
				pair<int,int> redoRobot = moveRobots.top();
				moveRobots.pop();
				robot[redoRobot.first].redoOneStep(collisionMap,redoRobot.second);
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
			// 已放下物品，对港口进行同步
			// 给港口的订单中物品数量+1
			this->harborWhoGotReceive.push_back(robot[i].atHarbor);
			harbor[robot[i].atHarbor].totalReceived++;
		}
		
	}
	// 机器人移动结束
	// 结算订单同步
	for (int i = 0; i < harborWhoGotReceive.size(); i++) {
		int harborId=harborWhoGotReceive[i];
		int boatId = hasBoat(harborId);
		// 若有船在港
		if (boatId!=-1) {
			// 给当前船的订单同步
			boat[boatId].addOneOrder();
			// 给港口的订单同步
			harbor[harborId].addOneOrder();
			harborWhoGotReceive.erase(harborWhoGotReceive.begin() + i);
			i--;
		}
	}
	if (harborWhoGotReceive.size() > 0) {
		// 如果是无船港口接收到新货物，单独同步
		for (int i = 0; i < harborWhoGotReceive.size(); i++) {
			int harborId = harborWhoGotReceive[0];
			// 首先查找是否有船要来
			if (harbor[harborId].orders.size() == 0) {
				// 没有船要来，不理他
			}
			else {
				// 有船要来，按照先来后到查看能否加给先来的船
				for (int j = 0; j < harbor[harborId].orders.size(); j++) {
					int boatId = harbor[harborId].orders[j].id;
					if (boat[boatId].getOrderCapacity() + 1 < boatCapacity) {
						// 放得下，同步
						boat[boatId].addOneOrder();
						harbor[harborId].addOneOrder();
						harborWhoGotReceive.erase(harborWhoGotReceive.begin() + i);
						i--;
						break;
					}
					else {
						// 放不下，不理他
					}
				}
			}
		}
	}
	// 单独筛一下港口
	//for (int i = 0; i < 10; i++) {
	//	if (harbor[i].orders.size()==0 && harbor[i].productPrices.size() >= boatCapacity)
	//		harbor[i].tiger = true;
	//}

	// 首先看船是否需要回去交货
	for (int i = 0; i < 5; i++) {
		// 当恰好装满或者时间不够时，立刻回去交货
		if (boat[i].getCurCapacity() == boatCapacity
			|| (boat[i].getPos() != -1 && boat[i].getStatus() == NORMAL && 15000 - frame < harbor[boat[i].getPos()].time)
			) {
			// 触发交货时，对订单进行同步
			clearWhenBoatComeBack(i, boat[i].getPos());
			harbor[boat[i].getPos()].prioTiger -= upperRate;
			boat[i].comeBack(frame);
			printDebug();
		}
	}
	// 然后看船是否需要换一个港口
	for (int i = 0; i < 5; i++) {
		// 当船停靠在某个港口，且该港口恰好没有堆积的货物可以给装载，寻找可换的港口（此处需要考虑换港口是否划算，不要去太少堆积的地方，或者干脆原地等待）
		if (boat[i].getPos() != -1 && boat[i].getStatus() == NORMAL && boat[i].getPre() == 0 && harbor[boat[i].getPos()].productPrices.size()==0) {
			int harborId = selectFastestHarbor(boatCapacity-boat[i].getCurCapacity(),harbor[boat[i].getPos()].time,boat[i].getCurValue(),i);
			// 若有可换的，立刻过去
			if (harborId != -1 && harborId!=-2) {
				harbor[boat[i].getPos()].prioTiger += upperRate;
				boat[i].gotoHarbor(harborId);

				// 触发换港口时，对订单进行同步
				synchronizeWhenSwitch(i, harborId);
				printDebug();
			}
			else if (harborId == -2) {
				continue;
			}
			// 如果是-1，说明没有合适的港口，立刻回去交货
			else {
				// 触发交货时，对订单进行同步
				clearWhenBoatComeBack(i, boat[i].getPos());
				harbor[boat[i].getPos()].prioTiger -= upperRate;
				boat[i].comeBack(frame);
				printDebug();
			}
		}
	}
	vector<int> freeBoat = getFreeBoat();
	// 根据空闲数量挑选货最多的港口
	for (int i = 0; i < freeBoat.size(); i++) {
		int harborId = selectAvailableFastestHarborWithGoingFromOriginPoint();
		// 要注意此处boat从交货地点出发，要考虑是否还有时间回来交货
		if (harborId != -1) {
			//cerr << "eff " << frame/boat[freeBoat[i]].turns << endl;
			if (15000 - frame < warningRate * frame / boat[freeBoat[i]].turns) {
				// 可以认为这是最后一次机会了
				boat[freeBoat[i]].atLast = true;
				//cerr<<"harbor "<<harborId<<" is the last chance"<<endl;
				harbor[harborId].prioTiger += upperRate*2;
			}
			boat[freeBoat[i]].gotoHarbor(harborId);
			
			// 出门时，对订单进行同步
			synchronizeWhenGoOut(freeBoat[i], harborId);
			printDebug();
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
	for (int j = 0; j < harbor[harborId].orders.size(); j++) {
		i += harbor[harborId].orders[j].productNumber;
	}
	int number = 0;
	for (; i < harbor[harborId].productPrices.size() && i<total; i++) {
		count += harbor[harborId].productPrices[i];
		number++;
	}
	// count为可在港口装载的货物总价值
	// 在此处判断时间是否还够换港口
	int loadcost = (number) / harbor[harborId].velocity;
	if (loadcost + harbor[harborId].time*2 + frame >= 15000) {
		return -1;
	}
	return count/float(loadcost+(harbor[harborId].time/(pow(frameImpactFactor, frame))*2));
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
int Scheduler::selectFastestHarbor(int countNeeded, int timeTocomeBack, int holdedValue, int boatId) {
	int bestID = -1;
	float value = -1;
	for (int i = 0; i < 10; i++) {
		// 计算未来的价值（单位：效率 钱/时间）
		float tmpValue = getFutureValue(i, countNeeded, boatId);
		// (处必须考虑，若该港口货物非常多，是否要排队，排队可能也是划算的)

		if (tmpValue > value) {
			value = tmpValue;
			bestID = i;
		}
	}
	
	return bestID;
}
/*
* @param harborId 目标港口id
* @param total    最大可装货物数量
*/
float Scheduler::getFutureValue(int harborId, int total, int boatId) {
	int count = 0;
	int i = 0;
	for (int j = 0; j < harbor[harborId].orders.size(); j++) {
		i+=harbor[harborId].orders[j].productNumber;
	}
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
	// 该harbor可提供的货物数量应该约等于countNeeded
	if (total - number < 500/(frame/harbor[harborId].totalReceived) || boat[boatId].atLast) {
		return count / float(loadcost + 500 + harbor[harborId].time/(pow(frameImpactFactor,frame)));
	}
	return -2;
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
	_boat->addGoods(num, value);
	_harbor->productPrices.erase(_harbor->productPrices.begin(), _harbor->productPrices.begin() + num);
	_harbor->orders[0].minus(num);

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

void Scheduler::clearWhenBoatComeBack(int boatId, int harborId) {
	this->boat[boatId].clearOrders();
	this->harbor[harborId].clearOneOrder();
}

void Scheduler::clearWhenBoatSwitch(int boatId, int harborId) {
	this->boat[boatId].clearOneOrder();
	this->harbor[harborId].clearOneOrder();
}

int Scheduler::hasBoat(int harborId) {
	for (int i = 0; i < 5; i++) {
		if (boat[i].getStatus() == NORMAL && boat[i].getPos() == harborId) {
			return i;
		}
	}
	return -1;
}

void Scheduler::synchronizeWhenGoOut(int boatId, int harborId) {
	int goodsLeft = harbor[harborId].productPrices.size();
	for (int i = 0; i < harbor[harborId].orders.size(); i++) {
		goodsLeft -= harbor[harborId].orders[i].productNumber;
	}
	boat[boatId].newOrder(goodsLeft,harborId);
	harbor[harborId].newOrder(goodsLeft,boatId);
}

void Scheduler::synchronizeWhenSwitch(int boatId, int harborId) {
	// 切换的不同是，需要清除原来的订单
	int prevHarbor = boat[boatId].originPos();
	// 对原来的港口同步
	harbor[prevHarbor].clearOneOrder();
	// 对新的港口同步
	int goodsLeft = harbor[harborId].productPrices.size();
	for (int i = 0; i < harbor[harborId].orders.size(); i++) {
		goodsLeft -= harbor[harborId].orders[i].productNumber;
	}
	boat[boatId].clearOneOrder();
	boat[boatId].newOrder(goodsLeft, harborId);
	harbor[harborId].newOrder(goodsLeft, boatId);

}





void Scheduler::printDebug() {
	for (int i = 0; i < 10; i++) {
		cerr << "harbor " << i << " has " << harbor[i].productPrices.size() << endl;
	}


	int a = 0;
}