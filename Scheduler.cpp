#include "Scheduler.h"

Scheduler::Scheduler() {
#ifdef TIME_DEBUG
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
#endif // TIME_DEBUG
	isbuy = 0;

	map = Map();
	this->robotNum = 0;
	for (int i = 0; i < 200; i++) {
		char line[201] = { 0 };
		scanf("%s", line);
		for (int j = 0; j < 200; j++) {
			map.setPoint(i, j, line[j]);
			switch (line[j])
			{
			case BUY_SHIP_SPACE:
				this->boatBuyPlace.push_back(Coord(i, j + 1));
				break;

			case BUY_ROBOT_SPACE:
				this->robotBuyPlace.push_back(Coord(i, j));
				break;
			}

		}
	}

	scanf("%d", &this->harborNum);
	vector<Coord> harborsCoord = vector<Coord>(harborNum);
	for (int i = 0; i < harborNum; i++) {
		int id, x, y, velocity;
		scanf("%d %d %d %d", &id, &x, &y, &velocity);
		Harbor curHarbor = Harbor(id, x, y, velocity);
		setBestBerthCoord(curHarbor, map.point);
		curHarbor.getBestCoord(map.point);
		harbors.push_back(curHarbor);
		harborsCoord[i] = curHarbor.robotCoord;
	}
	int sellPlace = harborNum;
	for (int i = 0; i < LEN; i++) {
		for (int j = 0; j < LEN; j++) {
			if (map.point[i][j] == DELIVERY) {
				boatDeliveryPlace.push_back(Delivery(Coord(i, j), sellPlace++));
			}
		}
	}

	pathPlanner.initHarborPath(map.point, harborsCoord);


	this->robots = vector<Robot>();
	this->boats = vector<Boat>();
	int Capacity = 0;
	scanf("%d", &Capacity);
	this->boatCapacity = Capacity;

	// 生成船运路线图
	this->boatPathPlanner = BoatPathPlanner(harborNum, boatDeliveryPlace.size());
	this->boatPathPlanner.init(map.point, harbors, boatDeliveryPlace);



	// 结尾
	char end[100];
	scanf("%s", end);
#ifdef TIME_DEBUG
	std::chrono::steady_clock::time_point ending = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(ending - start);
	double percent = time_span.count() / 15000 / 5 * 100;
	cerr << "Time used: " << time_span.count() << " " << setiosflags(ios::fixed) << setprecision(4) << percent << "%" << endl;
#endif // TIME_DEBUG

	printf("OK\n");
	fflush(stdout);
}

bool Scheduler::NextFrame() {
	if (frame == 14999)
	{
		printValue();
	}
	if (frame == 15000) {
		boatPathPlanner.clean();
		return false;
	}
	int money;
	if (!scanf("%d %d", &this->frame, &money)) // 读掉第一行frame和money
		return false;
	this->score = money;


	int changeGoodsCount;
	scanf("%d", &changeGoodsCount);
	for (int i = 0; i < changeGoodsCount; i++) {
		int x, y, val;
		scanf("%d %d %d", &x, &y, &val);
		if (val == 0) {
			continue;
		}
		vector<int> distanceToHarbors = vector<int>(harborNum);
		for (int j = 0; j < harborNum; j++)
		{
			distanceToHarbors[j] = pathPlanner.getDistanceToHarbor(j, Coord(x, y));
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

	scanf("%d", &robotNum);
	for (int i = 0; i < robotNum; i++) {
		int id, x, y, hasGoods;
		scanf("%d %d %d %d", &id, &hasGoods, &x, &y);
		robots[id].update(hasGoods, x, y);
	}
	scanf("%d", &this->boatNum);
	for (int i = 0; i < boatNum; i++) {
		int id, curCapacity, x, y, direction, status;
		scanf("%d %d %d %d %d %d", &id, &curCapacity, &x, &y, &direction, &status);
		//cerr << frame << ":" << id << ' ' << curCapacity << ' ' << x << ' ' << y << ' ' << direction << ' ' << status << '\n';
		boats[id].update(status, curCapacity, x, y, direction);
	}
	char end[100];
	scanf("%s", end); // 读掉结尾


	return true;
}

void Scheduler::findHarbor(int robotId)
{
	float minDistance = 10000;
	int bestHarborId = robots[robotId].goalHarbor;
	if (bestHarborId == -1 || harbors[bestHarborId].tiger) // 如果已经指定港口则不再查找
		for (int i = 0; i < harborNum; i++)
		{
			if (harbors[i].tiger)
				continue;
			float distance = pathPlanner.getDistanceToHarbor(i, Coord(robots[robotId].x, robots[robotId].y));
			if (distance > 0 && distance < minDistance)
			{
				minDistance = distance;
				bestHarborId = i;
			}

		}
	if (bestHarborId != -1)
	{
		if (robots[robotId].goalHarbor == -1) // 初始情况，原先没有位于港口
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
	for (int i = startProductId; i < products.size(); i++)
	{
		if (products[i].locked) // 产品被锁定
			continue;
		int nearestHarborId = products[i].getNearestHarborId();
		if (nearestHarborId == -1) // 产品没有可达港口
			continue;
		int pathLen1 = products[i].distanceToHarbors[startHarbor];
		if (pathLen1 == -1) // 产品不可到达机器人出发港口
			continue;
		if (frame + pathLen1 >= products[i].expireTime) // 机器人到达产品的时间超过了过期时间
			continue;
		int pathLen2 = products[i].distanceToHarbors[nearestHarborId];
		int pathLen = pathLen1 + pathLen2;
		double profitRate = (pow(products[i].price, 1 + robotId * valueImportance) / pathLen * exp(-expireTimeImportance * (products[i].expireTime - frame))
			+ harbors[nearestHarborId].getExpectedProfitRate(robotId, products[i].bestProfitRate) / productPerAgent * expectedWeight
			);
		if (profitRate > maxProfitRate)
		{
			maxProfitRate = profitRate;
			bestProductId = i;
			bestHarborId = nearestHarborId;
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
	//if (frame < 10)
	//{
	//	buyRobot(0);
	//}
	// 此处做决策，并输出指令
	if (frame >= 2 && this->score >= 2000 && isbuy < 8) {
		buyRobot(0);
		isbuy++;
	}

	for (int i = 0; i < robotNum; i++)
	{
		if (robots[i].target == -1)
		{
			//cerr << i << "findProductAndHarbor" << endl;
			findProductAndHarbor(i); //为第i个机器人分配任务
		}
		else if (robots[i].target == 1)
		{
			//cerr << i << "findHarbor" << endl;
			findHarbor(i); // 第i个机器人去最近的港口 
		}
	}
	//发出指令控制机器人移动
	// 碰撞图初始化
	int collisionMap[LEN][LEN] = { 0 };
	int safeMap[LEN][LEN] = { 0 };
	for (int i = 0; i < LEN; i++)
	{
		for (int j = 0; j < LEN; j++)
		{
			if (isRobotPath(map.point[i][j]))
				collisionMap[i][j] = 0;
			else
				collisionMap[i][j] = -1;
			if (isSafePath(map.point[i][j]))
				safeMap[i][j] = 0;
			else
				safeMap[i][j] = -1;
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
		restRobots.push(make_pair(i, -1)); // first是机器人id，second是机器人的移动方向
	}
	// 找到每个机器人的无冲突移动方向

	int countToughTask = 0;
	while (!restRobots.empty())
	{
		pair<int, int> curRobot = restRobots.top();
		restRobots.pop();

		pair<int, vector<int>> moveAndCollision = robots[curRobot.first].moveOneStep(collisionMap, safeMap);
		if (countToughTask <= 10)
			while (moveAndCollision.second.size())
			{
				//cerr << "tough task" << moveAndCollision.second.size() << ' ' << moveRobots.size() << endl;
				countToughTask++;
				pair<int, int> redoRobot = moveRobots.top();
				moveRobots.pop();
				robots[redoRobot.first].redoOneStep(collisionMap, redoRobot.second);
				auto iter = find(moveAndCollision.second.begin(), moveAndCollision.second.end(), redoRobot.first);
				if (iter != moveAndCollision.second.end())
					moveAndCollision.second.erase(iter);
				restRobots.push(redoRobot);
			}
		else
			while (moveAndCollision.second.size() && !moveRobots.empty())
			{
				//cerr << "really tough" << endl;
				pair<int, int> redoRobot = moveRobots.top();
				moveRobots.pop();
				robots[redoRobot.first].redoOneStep(collisionMap, redoRobot.second);
				restRobots.push(redoRobot);
			}
		curRobot.second = moveAndCollision.first;
		moveRobots.push(curRobot);
	}
	//发出指令控制机器人移动

	while (!moveRobots.empty())
	{
		pair<int, int> curRobot = moveRobots.top();
		moveRobots.pop();
		int i = curRobot.first;
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
	//// 机器人移动结束
	//// 结算订单同步

	for (int i = 0; i < harborWhoGotReceive.size(); i++) {
		int harborId = harborWhoGotReceive[i];
		int boatId = hasBoat(harborId);
		// 若有船在港
		if (boatId != -1) {
			// 给当前船的订单同步
			//cerr << boatId << endl;
			//cerr << &boats[boatId] << endl;
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


	if (frame == 1) {
		buyBoat(0);
	}

	for (int i = 0; i < boatNum; i++) {
		cerr << boats[i].pos << " " << boats[i].key << '\n';
		if (boats[i].isFree() && boats[i].status == MOVING) {
			// 没有目标的空闲船
			int targetId = findBestHarbor(boats[i]);
			cerr << "go to " << targetId << '\n';
			if (targetId >= harborNum) {

				clearWhenBoatComeBack(i, boats[i].preTarget);
				boats[i].target = targetId;
				boats[i].act(boatPathPlanner.nextMove(boats[i].key, boats[i].direction, boats[i].target, map.point));
			}
			else if (targetId != -1) {
				// 有的可去
				boats[i].target = targetId;
				if (boats[i].curCapacity == 0) {
					synchronizeWhenGoOut(i, targetId);
				}
				else
				{
					synchronizeWhenSwitch(i, targetId);
				}

				boats[i].act(boatPathPlanner.nextMove(boats[i].key, boats[i].direction, boats[i].target, map.point));

			}
		}
		else if (!boats[i].isFree() && atTarget(boats[i].pos, boats[i].target)) {
			// 抵达目标的船
			boats[i].act(DRIVEIN);
			if (boats[i].target >= harborNum) {
				boats[i].target = -1;
				boats[i].preTarget = -1;
			}
		}
		else if (!boats[i].isFree() && (boats[i].status == MOVING || boats[i].status == RECOVER)) {
			// 在去目标路上的船
			boats[i].act(boatPathPlanner.nextMove(boats[i].key, boats[i].direction, boats[i].target, map.point));

		}
		else if (!boats[i].isFree() && boats[i].status == LOADING) {
			// 在港口装货的船
			if (boats[i].curCapacity == boatCapacity || /*boats[i].preLoadNum == 0 &&*/ harbors[boats[i].target].productPrices.size() == 0) {
				// 没有货物了 或 装满了
				boats[i].act(LEAVE);
				boats[i].preTarget = boats[i].target;
				boats[i].target = -1;
			}
			else {
				loadGoods(&boats[i], &harbors[boats[i].target]);
			}
		}
		else {
			// 离开目标中
			// 什么也不做，等到变成空闲船

		}

	}





	// 结尾
	printf("OK\n");
	fflush(stdout);
}

int Scheduler::findBestHarbor(const Boat& boat)
{

	if (boat.curCapacity + 10 >= boatCapacity) {
		int mindistance = INT_MAX;
		int bestDeliveryPlace = -1;
		for (int i = harborNum; i < harborNum + boatDeliveryPlace.size(); i++)
		{
			int distance = boatPathPlanner.getDistance(boat.pos, i);
			if (distance < mindistance) {
				mindistance = distance;
				bestDeliveryPlace = i;
			}
		}
		return bestDeliveryPlace;
	}
	Coord pos = boat.pos;

	float max = 0;
	int bestHarborId = -1;
	for (int i = 0; i < harborNum; i++) {
		if (boat.preTarget == i)
			continue;

		int distance = boatPathPlanner.getDistance(pos, i);
		if (harbors[i].productPrices.size() / float(distance) > max) {
			max = harbors[i].productPrices.size() / float(distance);
			bestHarborId = i;
		}
	}
	return bestHarborId;
}

bool Scheduler::boatAtBuy(int boatId)
{
	Coord boatPos = boats[boatId].pos;
	for (int i = 0; i < boatBuyPlace.size(); i++)
	{
		if (boatPos + 0 == boatBuyPlace[i])
			return true;
	}
	return false;
}





void Scheduler::loadGoods(Boat* _boat, Harbor* _harbor)
{
	int num = min(min(_harbor->velocity, int(_harbor->productPrices.size())), boatCapacity - _boat->getCurCapacity());
	int value = 0;
	for (int i = 0; i < num; i++) {
		value += _harbor->productPrices[i];
	}
	_boat->addGoods(num, value);
	_harbor->productPrices.erase(_harbor->productPrices.begin(), _harbor->productPrices.begin() + num);
	_harbor->orders[0].minus(num);

}

void Scheduler::printValue() {
	for (int i = 0; i < harborNum; i++) {
		int count = 0;
		for (int j = 0; j < harbors[i].productPrices.size(); j++) {
			count += harbors[i].productPrices[j];
		}
		cerr << "harbor " << i << " " << count << endl;
	}
	cerr << "final " << score << endl;
}

void Scheduler::clearWhenBoatComeBack(int boatId, int harborId) {
	this->boats[boatId].clearOrders();
	this->harbors[harborId].clearOneOrder();
}

void Scheduler::clearWhenBoatSwitch(int boatId, int harborId) {
	this->boats[boatId].clearOneOrder();
	this->harbors[harborId].clearOneOrder();
}



void Scheduler::synchronizeWhenGoOut(int boatId, int harborId) {
	int goodsLeft = harbors[harborId].productPrices.size();
	for (int i = 0; i < harbors[harborId].orders.size(); i++) {
		goodsLeft -= harbors[harborId].orders[i].productNumber;
	}
	boats[boatId].newOrder(goodsLeft, harborId);
	harbors[harborId].newOrder(goodsLeft, boatId);
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


void Scheduler::buyBoat(int buyIndex)
{
	printf("lboat %d %d\n", boatBuyPlace[buyIndex].x, boatBuyPlace[buyIndex].y - 1);
	boats.push_back(Boat(buyIndex, boatCapacity));
}

void Scheduler::buyRobot(int buyIndex)
{
	printf("lbot %d %d\n", robotBuyPlace[buyIndex].x, robotBuyPlace[buyIndex].y);
	robots.push_back(Robot(robotNum, robotBuyPlace[buyIndex].x, robotBuyPlace[buyIndex].y));
}

void Scheduler::setBestBerthCoord(Harbor& curHarbor, char my_map[LEN][LEN])
{
	// 为当前港口设置最佳停靠点
	Coord point = curHarbor.boatCoord; // 判题器给出的坐标
	char tmp[5][5] = { { -1} };
	tmp[2][2] = 0; // 将该点作为中心点
	for (int i = -2; i <= 2; i++) {
		for (int j = -2; j <= 2; j++) {
			if (i == 0 && j == 0)
				continue;
			tmp[2 + i][2 + j] = -1; // -1表示该点非港口
			if (point.x + i < 0 || point.x + i >= LEN || point.y + j < 0 || point.y + j >= LEN)
				continue;
			char ch = my_map[point.x + i][point.y + j];
			if (ch == HARBOR_SPACE) {
				tmp[2 + i][2 + j] = 0;
			}
		}
	}

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (tmp[i][j] == 0) {
				// 找到第一个点，是左上角
				// 判断港口是横着还是竖着

				if (tmp[i][j + 2] == 0) {
					// 横着
					if (point.x + i - 1 >= 0 && my_map[point.x + i - 2 - 1][point.y + j - 2] == LOAD_SPACE) {
						// 上覆盖
						curHarbor.berthCoord = Coord(point.x + i - 2 - 2 + 1, point.y + j + 1 - 2);
					}
					else {
						// 下覆盖
						curHarbor.berthCoord = Coord(point.x + i + 3 - 2 - 1, point.y + j + 1 - 2);
					}
				}
				else {
					// 竖着
					if (point.y + j - 1 >= 0 && my_map[point.x + i - 2][point.y + j - 1 - 2] == LOAD_SPACE) {
						// 左覆盖
						curHarbor.berthCoord = Coord(point.x + i + 1 - 2, point.y + j - 2 - 2 + 1);
					}
					else {
						// 右覆盖
						curHarbor.berthCoord = Coord(point.x + i + 1 - 2, point.y + j + 3 - 2 - 1);
					}
				}
				return;
			}
		}
	}

}


bool Scheduler::atTarget(Coord pos, int targetId)
{
	if (targetId == -1)
	{
		return false;
	}
	Coord targetPos;
	if (targetId >= harborNum) {
		targetPos = boatDeliveryPlace[targetId - harborNum].getPos();
	}
	else {
		targetPos = harbors[targetId].berthCoord;
	}
	if (pos == targetPos) {
		return true;
	}
	return false;
}


int Scheduler::hasBoat(int harborId)
{
	for (int i = 0; i < boatNum; i++) {
		if (boats[i].target == harborId && boats[i].status == LOADING) {
			return i;
		}
	}
	return -1;
}