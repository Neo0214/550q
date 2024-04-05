#include "Scheduler.h"

Scheduler::Scheduler() {
#ifdef TIME_DEBUG
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
#endif // TIME_DEBUG

	int sellPlace = -1;
	map = Map();
	this->robotNum = 0;
	for (int i = 0; i < 200; i++) {
		char line[201] = {0};
		scanf("%s", line);
		for (int j = 0; j < 200; j++) {
			map.setPoint(i, j, line[j]);
			switch (line[j])
			{
			case BUY_SHIP_SPACE:
				this->boatBuyPlace.push_back(Coord(i, j + 1));
				break;
			case DELIVERY:
				harbors.push_back(Harbor(sellPlace--, i, j, 0));
				break;
			}
			
		}
	}
	
	scanf("%d", &this->harborNum);
	for (int i = 0; i < harborNum; i++) {
		int id, x, y, velocity;
		scanf("%d %d %d %d", &id, &x, &y, &velocity);
		harbors.push_back(Harbor(id, x, y, velocity));
	}
	// 挪一下，把harbor挪到前半，-n交货港挪到后面
	vector<Harbor> tmp=vector<Harbor>();
	tmp.insert(tmp.end(), harbors.begin(), harbors.begin() + (-sellPlace) - 1);
	harbors.erase(harbors.begin(), harbors.begin() + (-sellPlace) - 1);
	harbors.insert(harbors.end(), tmp.begin(), tmp.end());
	this->robots=vector<Robot>();
	this->boats=vector<Boat>();
	int Capacity = 0;
	scanf("%d", &Capacity);
	this->boatCapacity = Capacity;

	// 生成船运路线图
	this->boatPathPlanner=BoatPathPlanner();
	this->boatPathPlanner.initBoatPathPlanner(map.point, harbors, boatBuyPlace, harborNum);

	// 结尾
	char end[100];
	scanf("%s",end);
#ifdef TIME_DEBUG
	std::chrono::steady_clock::time_point ending = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(ending - start);
	double percent = time_span.count() / 15000 / 5 * 100;
	cerr << "Time used: " <<time_span.count()<<" "<< setiosflags(ios::fixed) << setprecision(4) << percent << "%" << endl;
#endif // TIME_DEBUG

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

	int changeGoodsCount;
	scanf("%d", &changeGoodsCount);
	for (int i = 0; i < changeGoodsCount; i++) {
		int x, y, val;
		scanf("%d %d %d", &x, &y, &val);
		if (val == 0) {
			continue;
		}
		vector<int> distanceToHarbors=vector<int>(harborNum);
		for (int j = 0; j < harborNum; j++)
		{
			distanceToHarbors[i]=pathPlanner.getDistanceToHarbor(i,Coord(x,y)); 
		}
		Product newProduct = Product(x, y, val, frame + 1000, distanceToHarbors);
		products.push_back(newProduct);

		int nearestHarborId = newProduct.getNearestHarborId();
		if (nearestHarborId != -1) // 产品有可达港口
			harbors[nearestHarborId].appendProfitRate(newProduct.bestProfitRate);
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
	int robotCount;
	scanf("%d",&robotCount);
	for (int i = 0; i < robotCount; i++) {
		int id, x, y, hasGoods;
		scanf("%d %d %d %d", &id, &x, &y, &hasGoods);
		robots[id].update(hasGoods, x, y);
	}
	int harborCount;
	scanf("%d", &harborCount);
	for (int i = 0; i < harborCount; i++) {
		int id, curCapacity, x, y, direction, status;
		scanf("%d %d %d %d %d %d", &id, &curCapacity, &x, &y, &direction, &status);
		boats[id].update(status,curCapacity, x, y, direction);
	}

	char end[100];
	scanf("%s", end); // 读掉结尾
	
	
	return true;
}

void Scheduler::findHarbor(int robotId)
{
	float minDistance = 10000;
	int bestHarborId = robots[robotId].goalHarbor;
	if(bestHarborId==-1 || harbors[bestHarborId].tiger) // 如果已经指定港口则不再查找
		for (int i = 0; i < harborNum; i++)
		{
			if (harbors[i].tiger)
				continue;
			float distance = pathPlanner.getDistanceToHarbor(i, Coord(robots[robotId].x, robots[robotId].y));
			if (distance >0 && distance < minDistance)
			{
				minDistance = distance;
				bestHarborId = i;
			}

		}
	if (bestHarborId != -1)
	{
		if(robots[robotId].goalHarbor==-1) // 初始情况，原先没有位于港口
			harbors[bestHarborId].appendRobot(robotId);
		else if (robots[robotId].goalHarbor != bestHarborId) // 正常来说不会进入此if，除非在findHarbor中目标港口发生变化
		{
			harbors[robots[robotId].goalHarbor].removeRobot(robotId);
			harbors[bestHarborId].appendRobot(robotId);
		}
		robots[robotId].assignTask(pathPlanner.getPathToHarbor(bestHarborId, Coord(robots[robotId].x, robots[robotId].y)), 2, bestHarborId, bestHarborId);
	}

}

void Scheduler::findProductAndHarbor(int robotId)
{
	if (robots[robotId].moves.size() != robots[robotId].curMoveCount) // 如果还有未走完的路不要设置目标
		return;
	int startHarbor = robots[robotId].atHarbor; // 将此前findHarbor设置的目标港口作为出发港口

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
			+ harbors[nearestHarborId].getExpectedProfitRate(robotId, products[i].bestProfitRate) / productPerAgent * expectedWeight
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
		if (robots[robotId].goalHarbor != bestHarborId) // 机器人目标港口发生变化
		{
			harbors[robots[robotId].goalHarbor].removeRobot(robotId);
			harbors[bestHarborId].appendRobot(robotId);
		}
		robots[robotId].assignTask(pathPlanner.getPathFromHarbor(startHarbor, Coord(products[bestProductId].x, products[bestProductId].y)), 0, -1, bestHarborId);
		products[bestProductId].locked = true;
		harbors[bestHarborId].removeProfitRate(products[bestProductId].bestProfitRate); // 商品锁定后，港口对应收益率减少
		robots[robotId].carryProduct = bestProductId;
	}

}


void Scheduler::Update() {
	// 此处做决策，并输出指令
	for (int i = 0; i <robotNum; i++) 
	{
		if (robots[i].target == -1) 
		{
			findProductAndHarbor(i); //为第i个机器人分配任务
		}
		else if (robots[i].target == 1)
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
	for (int i = 0; i < robotNum; i++)
	{
		collisionMap[robots[i].x][robots[i].y] = i + 1; // 注意这里是i+1，因为机器人id从0开始
	}
	// 移动栈和剩余栈
	stack<pair<int, int>> moveRobots;
	stack<pair<int, int>> restRobots;
	// 剩余栈初始化
	for (int i = robotNum - 1; i >= 0; i--) 
	{
		restRobots.push(make_pair(i,-1)); // first是机器人id，second是机器人的移动方向
	}
	// 找到每个机器人的无冲突移动方向
	int countToughTask = 0;
	while(!restRobots.empty())
	{
		pair<int,int> curRobot = restRobots.top();
		restRobots.pop();
		
		pair<int, vector<int>> moveAndCollision = robots[curRobot.first].moveOneStep(collisionMap);
		if(countToughTask<=10)
			while (moveAndCollision.second.size())
			{
				//cerr << "tough task" << moveAndCollision.second.size() <<' ' << moveRobots.size() <<endl;
				countToughTask++;
				pair<int,int> redoRobot = moveRobots.top();
				moveRobots.pop();
				robots[redoRobot.first].redoOneStep(collisionMap,redoRobot.second);
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
				robots[redoRobot.first].redoOneStep(collisionMap,redoRobot.second);
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
		int action = robots[i].command(curRobot.second);
		if (action == 1) //放下物品
		{
			harbors[robots[i].atHarbor].productPrices.push_back(products[robots[i].carryProduct].price);
			score += products[robots[i].carryProduct].price;
			// 已放下物品，对港口进行同步
			// 给港口的订单中物品数量+1
			this->harborWhoGotReceive.push_back(robots[i].atHarbor);
			harbors[robots[i].atHarbor].totalReceived++;
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
			boats[boatId].addOneOrder();
			// 给港口的订单同步
			harbors[harborId].addOneOrder();
			harborWhoGotReceive.erase(harborWhoGotReceive.begin() + i);
			i--;
		}
	}
	if (harborWhoGotReceive.size() > 0) {
		// 如果是无船港口接收到新货物，单独同步
		for (int i = 0; i < harborWhoGotReceive.size(); i++) {
			int harborId = harborWhoGotReceive[0];
			// 首先查找是否有船要来
			if (harbors[harborId].orders.size() == 0) {
				// 没有船要来，不理他
			}
			else {
				// 有船要来，按照先来后到查看能否加给先来的船
				for (int j = 0; j < harbors[harborId].orders.size(); j++) {
					int boatId = harbors[harborId].orders[j].id;
					if (boats[boatId].getOrderCapacity() + 1 < boatCapacity) {
						// 放得下，同步
						boats[boatId].addOneOrder();
						harbors[harborId].addOneOrder();
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
	
		



	// 结尾
	printf("OK\n");
	fflush(stdout);
}

int Scheduler::getFutureValueFromOriginPoint(int harborId, int total) {
	int count = 0;
	int i = 0;
	for (int j = 0; j < harbors[harborId].orders.size(); j++) {
		i += harbors[harborId].orders[j].productNumber;
	}
	int number = 0;
	for (; i < harbors[harborId].productPrices.size() && i<total; i++) {
		count += harbors[harborId].productPrices[i];
		number++;
	}
	// count为可在港口装载的货物总价值
	// 在此处判断时间是否还够换港口
	int loadcost = (number) / harbors[harborId].velocity;
	if (loadcost + frame >= 15000) {
		return -1;
	}
	return count/float(loadcost);
}

int Scheduler::selectAvailableFastestHarborWithGoingFromOriginPoint() {
	int bestID = -1;
	float value = -1;
	for (int i = 0; i < 10; i++) {
		if (frame>=15000) {
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
	for (int j = 0; j < harbors[harborId].orders.size(); j++) {
		i+=harbors[harborId].orders[j].productNumber;
	}
	int number = 0;
	for (; i < harbors[harborId].productPrices.size() && number<total; i++) {
		count += harbors[harborId].productPrices[i];
		number++;
	}
	// count为可在港口装载的货物总价值
	// 在此处判断时间是否还够换港口
	int loadcost = number / harbors[harborId].velocity;
	if (loadcost +  500 + frame >= 15000) {
		return -1;
	}
	// 该harbor可提供的货物数量应该约等于countNeeded
	if (total - number < 500/(frame/harbors[harborId].totalReceived) || boats[boatId].atLast) {
		return count / float(loadcost + 500);
	}
	return -2;
}



vector<int> Scheduler::getFreeBoat() {
	vector<int> boatIds;
	for (int i = 0; i < 5; i++) {
		if (boats[i].getPos() == -1 && boats[i].getStatus()==NORMAL)
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
	for (int i = 0; i < harborNum; i++) {
		int count = 0;
		for (int j = 0; j < harbors[i].productPrices.size(); j++) {
			count+=harbors[i].productPrices[j];
		}
		cerr<<"harbor "<<i<<" "<<count<<endl;
	}
	cerr <<"final " << score << endl;
}

void Scheduler::clearWhenBoatComeBack(int boatId, int harborId) {
	this->boats[boatId].clearOrders();
	this->harbors[harborId].clearOneOrder();
}

void Scheduler::clearWhenBoatSwitch(int boatId, int harborId) {
	this->boats[boatId].clearOneOrder();
	this->harbors[harborId].clearOneOrder();
}

int Scheduler::hasBoat(int harborId) {
	for (int i = 0; i < 5; i++) {
		if (boats[i].getStatus() == NORMAL && boats[i].getPos() == harborId) {
			return i;
		}
	}
	return -1;
}

void Scheduler::synchronizeWhenGoOut(int boatId, int harborId) {
	int goodsLeft = harbors[harborId].productPrices.size();
	for (int i = 0; i < harbors[harborId].orders.size(); i++) {
		goodsLeft -= harbors[harborId].orders[i].productNumber;
	}
	boats[boatId].newOrder(goodsLeft,harborId);
	harbors[harborId].newOrder(goodsLeft,boatId);
}

void Scheduler::synchronizeWhenSwitch(int boatId, int harborId) {
	// 切换的不同是，需要清除原来的订单
	int prevHarbor = boats[boatId].originPos();
	// 对原来的港口同步
	harbors[prevHarbor].clearOneOrder();
	// 对新的港口同步
	int goodsLeft = harbors[harborId].productPrices.size();
	for (int i = 0; i < harbors[harborId].orders.size(); i++) {
		goodsLeft -= harbors[harborId].orders[i].productNumber;
	}
	boats[boatId].clearOneOrder();
	boats[boatId].newOrder(goodsLeft, harborId);
	harbors[harborId].newOrder(goodsLeft, boatId);

}





void Scheduler::printDebug() {
	for (int i = 0; i < 10; i++) {
		cerr << "harbor " << i << " has " << harbors[i].productPrices.size() << endl;
	}


	int a = 0;
}