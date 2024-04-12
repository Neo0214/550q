#include "Scheduler.h"

Scheduler::Scheduler() {
#ifdef TIME_DEBUG
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
#endif // TIME_DEBUG

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
				this->boatBuyPlace.push_back(Coord(i, j));
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

	robotBuyList = getRobotBuyList();
	for (int i = 0; i < robotBuyList.size(); i++) {}
	//cerr << robotBuyList[i] << ' ';
//cerr << endl;
	boatBuyList = getBoatBuyList();
	for (int i = 0; i < boatBuyList.size(); i++) {}
	//cerr << boatBuyList[i] << ' ';
//cerr << endl;

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
	if (robotNum > robots.size())
	{
		for (int i = robots.size(); i < robotNum; i++)
		{
			robots.push_back(Robot(i, 0, 0));
		}

	}
	for (int i = 0; i < robotNum; i++) {
		int id, x, y, hasGoods;
		scanf("%d %d %d %d", &id, &hasGoods, &x, &y);
		robots[id].update(hasGoods, x, y);
	}


	scanf("%d", &this->boatNum);
	if (boatNum > boats.size())
	{
		for (int i = boats.size(); i < boatNum; i++)
		{
			boats.push_back(Boat(i, boatCapacity));
		}
	}
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
	//cerr << frame << endl;
	//if (frame < 10)
	//{
	//	buyRobot(0);
	//}
	// 此处做决策，并输出指令
	if (boatNum < boatBuyList.size()) {
		buyBoat(boatNum);
	}
	if (robotNum < robotBuyList.size()) {
		buyRobot(robotBuyList[robotNum]);
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
			robotTotalScore += products[robots[i].carryProduct].price;
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

	//cerr << "frame:" << frame << endl;
	for (int i = 0; i < boatNum; i++) {
		if (boats[i].isFree() && boats[i].status == MOVING) {
			// 没有目标的空闲船
			//cerr << "frame " << frame << endl;
			vector<int> targetId = findBestHarbor(boats[i]);
			//cerr << targetId.size() << endl;
			//cerr << "boat " << i << " go to ";
			//for (int k = 0; k < targetId.size(); k++)
			//	cerr << targetId[k] << " ";
			//cerr << endl;

			if (targetId.size() == 2) {
				// 选择交货再出发
				clearWhenBoatComeBack(i, boats[i].preTarget);
				synchronizeWhenGoOut(i, targetId[1]);
				//cerr << "go find a path" << endl;
				boats[i].target = targetId;
				boats[i].action = boatPathPlanner.getPathWithVia(BoatState(boats[i].pos, boats[i].direction), targetId[0], targetId[1], i, boats[i].viaIndex);
				boats[i].curAct = 0;
				//cerr << "go find a path-ok" << endl;
			}
			else if (targetId.size() == 1 && targetId[0] != -1) {
				// 有的可去
				boats[i].target = targetId;
				if (boats[i].curCapacity == 0) {
					synchronizeWhenGoOut(i, targetId[0]);
				}
				else
				{
					synchronizeWhenSwitch(i, targetId[0]);
				}
				boats[i].action = boatPathPlanner.getPath(BoatState(boats[i].pos, boats[i].direction), targetId[0], i);
				boats[i].curAct = 0;
			}
		}
		else if (!boats[i].isFree() && (boats[i].status == MOVING/* || boats[i].status == RECOVER*/)) {
			// 在去目标路上的船
			//cerr << "do nextact" << endl;
			boats[i].nextAct(harborNum, &boatPathPlanner);
			//cerr << "do nextact-ok" << endl;
		}
		else if (!boats[i].isFree() && boats[i].status == LOADING) {
			// 在港口装货的船
			//cerr << "in harbor" << endl;
			//cerr << boats[i].target[0] << endl;
			if (boats[i].target[0] < harborNum && (boats[i].curCapacity == boatCapacity || /*boats[i].preLoadNum == 0 &&*/ harbors[boats[i].target[0]].productPrices.size() == 0)) {
				// 没有货物了 或 装满了
				boats[i].act(LEAVE);
				boats[i].preTarget = boats[i].target[0];
				boats[i].target = vector<int>();
			}
			else if (boats[i].target[0] < harborNum) {
				loadGoods(&boats[i], &harbors[boats[i].target[0]]);
			}
			//cerr << "in harbor ok" << endl;
		}
		else {
			// 离开目标中
			// 什么也不做，等到变成空闲船

		}

	}
	//cerr << "frame-ok" << endl;




	// 结尾
	printf("OK\n");
	fflush(stdout);
}

vector<int> Scheduler::findBestHarbor(Boat& boat)
{
	//cerr << "try find" << endl;
	vector<int> bestIds = vector<int>();
	if (boat.isFirst) {
		float effi = 0;
		int bestId = -1;
		int nosense = 0;
		for (int i = 0; i < harborNum; i++) {
			int totalValue = getValue(boatCapacity, i, nosense);
			int distance = boatPathPlanner.getDistance(BoatState(boat.pos, boat.direction), i);
			float curEffi = totalValue / distance;
			if (curEffi > effi) {
				effi = curEffi;
				bestId = i;
				boat.isFirst = false;
			}
		}
		bestIds.push_back(bestId);
		return bestIds;
	}
	// 以下是常规切换挑选
	//cerr << "try find-1" << endl;
	float effi = 0;
	int bestId = -1;
	if (boat.preTarget < harborNum) {
		// 说明是从港口出发的，需要考虑是否去交货
		// 首先是选择港口的情况
		//cerr << "try find-2" << endl;
		for (int i = 0; i < harborNum; i++) {
			if (i == boat.preTarget) {
				continue;
			}
			int loadFrame = 0; // 装载时间
			int totalValue = getValue(boatCapacity - boat.curCapacity, i, loadFrame);
			int distance = boatPathPlanner.getDistance(BoatState(boat.pos, boat.direction), i);
			float curEffi = totalValue / distance;
			if (curEffi > effi && timeOKFromHarbor(boat.preTarget, i, loadFrame)) {
				effi = curEffi;
				bestId = i;
			}
		}
		// 在选择港口的情况下，能不能途径其他点？
		//cerr << "try find-3" << endl;
		if (bestId != -1) {
			if (boatPathPlanner.getNextId(boat.preTarget, bestId) != bestId && boatPathPlanner.getNextId(boat.preTarget, bestId) < harborNum) {
				// 说明有途径情况
				int tmpId = boatPathPlanner.getNextId(boat.preTarget, bestId);
				int loadFrame = 0;
				int totalValue = getValue(boatCapacity - boat.curCapacity, tmpId, loadFrame);
				int distance = boatPathPlanner.getDistance(BoatState(boat.pos, boat.direction), tmpId);
				float curEffi = totalValue / distance;
				if (curEffi > effi * 0.6 && timeOKFromHarbor(boat.preTarget, tmpId, loadFrame)) {
					effi = curEffi;
					bestId = tmpId;
				}
				//cerr << "push " << bestId << endl;
				bestIds.push_back(bestId);
				return bestIds;
			}
			else {
				// 途径是交货点
				bestId = boatPathPlanner.getNextId(boat.preTarget, bestId);
				//cerr << "push " << bestId << endl;
				bestIds.push_back(bestId);
			}
		}
		else if (boat.curCapacity >= boatCapacity * 0.9) {
			// 没有合适港口，只能去交货点
			int minDistance = INT_MAX;
			for (int i = harborNum; i < harborNum + boatDeliveryPlace.size(); i++) {
				int distance = boatPathPlanner.getDistance(BoatState(boat.pos, boat.direction), i);
				if (distance < minDistance) {
					minDistance = distance;
					bestId = i;
				}
			}
			//cerr << "push " << bestId << endl;
			bestIds.push_back(bestId);
		}
		//cerr << "try find-4" << endl;
		if (bestId >= harborNum) {
			//cerr << "try find-5" << endl;
			bestId = 0; // 如果后面选不出来，强制选0号港
			for (int i = 0; i < harborNum; i++) {
				int loadFrame = 0; // 装载时间
				int totalValue = getValue(boatCapacity, i, loadFrame);
				int distance = boatPathPlanner.getCrossedDistance(bestIds[0], i);
				float curEffi = totalValue / distance;
				if (curEffi > effi && timeOKFromDelivery(boat.preTarget, i, loadFrame)) {
					effi = curEffi;
					bestId = i;
				}
			}
			//cerr << "try find-5" << endl;
			bestIds.push_back(bestId);
			return bestIds;
		}
		return bestIds;
	}

	bestIds.push_back(-1);
	return bestIds;
}

// 还能装多少，从哪个港口装
int Scheduler::getValue(int canLoadNum, int harborId, int& loadFrame)
{
	int left = harbors[harborId].productPrices.size();
	for (int i = 0; i < harbors[harborId].orders.size(); i++) {
		left -= harbors[harborId].orders[i].productNumber;
	}
	// left就是还剩多少货物未被预订
	int value = 0;
	int start = harbors[harborId].productPrices.size() - left;
	for (int i = 0; i < min(left, canLoadNum); i++) {
		value += harbors[harborId].productPrices[i + start];
	}
	loadFrame = min(left, canLoadNum) / harbors[harborId].velocity + min(left, canLoadNum) % harbors[harborId].velocity == 0 ? 0 : 1;
	return value;
}

bool Scheduler::timeOKFromDelivery(int deliveryId, int harborId, int loadFrame)
{
	int timeGoToHarbor = boatPathPlanner.getCrossedDistance(deliveryId, harborId);
	int timeToComeBack = 15000;
	for (int i = harborNum; i < harborNum + boatDeliveryPlace.size(); i++) {
		if (boatPathPlanner.getCrossedDistance(harborId, i) < timeToComeBack) {
			timeToComeBack = boatPathPlanner.getCrossedDistance(harborId, i);
		}
	}
	if (frame + timeGoToHarbor + timeToComeBack + loadFrame + 30 < 15000) {
		return true;
	}
	return false;
}
bool Scheduler::timeOKFromHarbor(int harborIdOrigin, int harborId, int loadFrame)
{
	int timeGoToHarbor = boatPathPlanner.getCrossedDistance(harborIdOrigin, harborId);
	int timeToComeBack = 15000;
	for (int i = harborNum; i < harborNum + boatDeliveryPlace.size(); i++) {
		if (boatPathPlanner.getCrossedDistance(harborId, i) < timeToComeBack) {
			timeToComeBack = boatPathPlanner.getCrossedDistance(harborId, i);
		}
	}
	if (frame + timeGoToHarbor + timeToComeBack + loadFrame + 30 < 15000) {
		return true;
	}
	return false;
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
	cerr << "final " << robotTotalScore - robotNum * 2000 - boatNum * 8000 + 25000 << endl;
}

void Scheduler::clearWhenBoatComeBack(int boatId, int harborId) {
	if (harborId >= harborNum)
		return;
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
	printf("lboat %d %d\n", boatBuyPlace[buyIndex].x, boatBuyPlace[buyIndex].y);
	//boats.push_back(Boat(boatNum, boatCapacity));
}

void Scheduler::buyRobot(int buyIndex)
{
	printf("lbot %d %d\n", robotBuyPlace[buyIndex].x, robotBuyPlace[buyIndex].y);
	//robots.push_back(Robot(robotNum, robotBuyPlace[buyIndex].x, robotBuyPlace[buyIndex].y));
}

void Scheduler::setBestBerthCoord(Harbor& curHarbor, char my_map[LEN][LEN])
{
	queue<Coord> q;
	q.push(curHarbor.boatCoord);
	int visited[LEN][LEN] = { 0 };
	visited[curHarbor.boatCoord.x][curHarbor.boatCoord.y] = 1;
	while (!q.empty())
	{
		Coord cur = q.front();
		q.pop();
		if (isHarbor(my_map[cur.x][cur.y]))
		{
			curHarbor.circleCoord.push_back(cur);
		}
		for (int i = 0; i < 4; i++)
		{
			Coord next = cur + i;
			if (isHarbor(my_map[next.x][next.y]) && !visited[next.x][next.y])
			{
				visited[next.x][next.y] = 1;
				q.push(next);
			}
		}
	}

}




int Scheduler::hasBoat(int harborId)
{
	for (int i = 0; i < boatNum; i++) {
		if (boats[i].target.size() > 0 && boats[i].target[0] == harborId && boats[i].status == LOADING) {
			return i;
		}
	}
	return -1;
}

// 修改了这里
vector<int> Scheduler::getRobotBuyList()
{
	// 计算各个连通区域中的港口
	vector<vector<int>> connectedHarbors;
	for (int i = 0; i < harborNum; i++)
	{
		int j;
		for (j = 0; j < connectedHarbors.size(); j++)
		{
			if (pathPlanner.getDistanceToHarbor(i, harbors[connectedHarbors[j][0]].boatCoord) >= 0)
			{
				connectedHarbors[j].push_back(i);
				break;
			}
		}
		if (j == connectedHarbors.size())
			connectedHarbors.push_back(vector<int>{i});
	}

	//// 打印各个连通区域中的港口
	//cerr<<"connectedHarbors"<<endl;
	//for (auto& connectedHarbor : connectedHarbors)
	//{
	//	for (auto& harborId : connectedHarbor)
	//	{
	//		cerr << harborId << ' ';
	//	}
	//	cerr << endl;
	//}
	//cerr << endl;

	//计算各连通区域的有效面积
	vector<int> effectArea;
	for (auto& connectedHarbor : connectedHarbors)
	{
		effectArea.push_back(pathPlanner.getEffectiveArea(connectedHarbor[0]));
	}
	int effectAreaSum = accumulate(effectArea.begin(), effectArea.end(), 0);
	//cerr<<"effectAreaSum" << effectAreaSum << endl;
	//// 打印各连通区域的有效面积
	//cerr << "effectArea" << endl;
	//for (auto& area : effectArea)
	//{
	//	cerr << area << ' ';
	//}
	//cerr<< endl;

	//计算机器人购买点所在的连通区域id
	vector<int> robotBuyPlaceInPosition(robotBuyPlace.size(), -1);
	for (int i = 0; i < robotBuyPlace.size(); i++)
	{
		for (int j = 0; j < connectedHarbors.size(); j++)
		{
			auto connectedHarbor = connectedHarbors[j];
			if (pathPlanner.getDistanceToHarbor(connectedHarbor[0], robotBuyPlace[i]) != -1)
			{ //第i个buyPlace位于第j个连通区内
				robotBuyPlaceInPosition[i] = j;
				//cerr << "place"<<i<<"in"<<j  << endl;
				break;
			}
		}
	}

	// 计算每一个连通区域内有多少个机器人购买点
	vector<int> robotBuyPlaceCount(connectedHarbors.size(), 0);
	for (int i = 0; i < robotBuyPlace.size(); i++)
	{
		int conectedAreaId = robotBuyPlaceInPosition[i];
		if (conectedAreaId != -1) // 机器人不在没有港口的区域内
		{
			robotBuyPlaceCount[conectedAreaId]++;
		}
	}

	//计算机器人购买点到其最近的港口的距离
	vector<int> robotBuyPlaceDistanceToHarbor(robotBuyPlace.size(), -1);
	for (int i = 0; i < robotBuyPlace.size(); i++)
	{
		int minDistance = 10000;
		int conectedAreaId = robotBuyPlaceInPosition[i];
		if (conectedAreaId != -1) // 机器人不在没有港口的区域内
		{
			for (auto harborId : connectedHarbors[conectedAreaId])
			{
				int distance = pathPlanner.getDistanceToHarbor(harborId, robotBuyPlace[i]); //连通区域内的港口到购买点的距离
				if (distance < minDistance)
				{
					minDistance = distance;
				}
			}
			robotBuyPlaceDistanceToHarbor[i] = minDistance;
		}
	}

	//每个连通区域内各个购买点按照距离计算排名
	vector<int> robotBuyPlaceRank(robotBuyPlace.size(), -1);
	for (int i = 0; i < robotBuyPlace.size(); i++)
	{
		int conectedAreaId = robotBuyPlaceInPosition[i];
		if (conectedAreaId != -1) // 机器人不在没有港口的区域内
		{
			int rank = 0;
			for (int j = 0; j < robotBuyPlace.size(); j++)
			{
				if (robotBuyPlaceInPosition[j] == conectedAreaId && robotBuyPlaceDistanceToHarbor[j] < robotBuyPlaceDistanceToHarbor[i] ||
					(robotBuyPlaceDistanceToHarbor[j] == robotBuyPlaceDistanceToHarbor[i]) && j < i)
				{
					rank++;
				}
			}
			robotBuyPlaceRank[i] = (1 << rank);
		}
	}




	// 计算分配比例
	vector<double> robotBuyPlaceProportion(robotBuyPlace.size(), 0);
	for (int i = 0; i < robotBuyPlace.size(); i++)
	{
		int areaId = robotBuyPlaceInPosition[i];

		if (areaId != -1)
		{
			robotBuyPlaceProportion[i] = (double(effectArea[areaId]) / effectAreaSum) * (1.0 / robotBuyPlaceRank[i] / (2 - 1.0 / robotBuyPlaceCount[areaId]));
		}
	}

	//// 打印分配比例
	//for (int i = 0; i < robotBuyPlace.size(); i++)
	//{
	//	cerr << robotBuyPlaceProportion[i] << ' ';
	//}

	int expectedRobotNum = effectAreaSum / 1900;

	// 计算每个购买点的购买数量
	vector<int> robotBuyPlacePurchaseNum(robotBuyPlace.size(), 0);
	for (int i = 0; i < robotBuyPlace.size(); i++)
	{
		robotBuyPlacePurchaseNum[i] = robotBuyPlaceProportion[i] * expectedRobotNum;
		//cerr<< robotBuyPlacePurchaseNum[i] << ' ';
	}
	//cerr << endl;

	// 计算每个区域购买的机器人数量
	vector<int> robotBuyPlaceCountSum(connectedHarbors.size(), 0);
	for (int i = 0; i < robotBuyPlace.size(); i++)
	{
		int areaId = robotBuyPlaceInPosition[i];
		if (areaId != -1)
		{
			robotBuyPlaceCountSum[areaId] += robotBuyPlacePurchaseNum[i];
		}
	}
	// 如果购买数量为0，则买一个
	for (int i = 0; i < robotBuyPlaceCountSum.size(); i++)
	{
		if (robotBuyPlaceCountSum[i] == 0)
		{
			auto buyPlaceIter = find(robotBuyPlaceInPosition.begin(), robotBuyPlaceInPosition.end(), i);
			int id = buyPlaceIter - robotBuyPlaceInPosition.begin();
			if (id != robotBuyPlace.size())
				robotBuyPlacePurchaseNum[id]++;
		}
	}

	int factualRobotNum = accumulate(robotBuyPlacePurchaseNum.begin(), robotBuyPlacePurchaseNum.end(), 0);
	//cerr<< "factualRobotNum" << factualRobotNum << endl;


	//生成一个购买数组
	vector<int>purchaseList;
	for (int i = 0; i < robotBuyPlace.size(); i++)
	{
		for (int j = 0; j < robotBuyPlacePurchaseNum[i]; j++)
		{
			purchaseList.push_back(i);
		}
	}

	// 随机打乱购买数组
	auto shuffleVector = [](std::vector<int>& vec) {
		for (size_t i = vec.size() - 1; i > 0; --i) {
			// 生成[0, i]范围内的随机数
			size_t j = rand() % (i + 1);
			// 交换元素
			std::swap(vec[i], vec[j]);
		}
		};

	// 调用lambda函数打乱向量
	shuffleVector(purchaseList);

	return purchaseList;
}

vector<int> Scheduler::getBoatBuyList()
{
	// 船的购买总个数为80/capacity
	// 每个有港口的连通区域至少买一辆
	// 每个连通区域只选取离港口最近的购买点买
	// 最初时只买一辆船，在港口最多的海域买
	// 第二艘在港口数次之的海域买
		// 计算各个连通区域中的港口
	vector<vector<int>> connectedHarbors;
	for (int i = 0; i < harborNum; i++)
	{
		int j;
		for (j = 0; j < connectedHarbors.size(); j++)
		{
			if (boatPathPlanner.getDistanceToHarbor(i, harbors[connectedHarbors[j][0]].boatCoord) >= 0)
			{
				connectedHarbors[j].push_back(i);
				break;
			}
		}
		if (j == connectedHarbors.size())
			connectedHarbors.push_back(vector<int>{i});
	}

	//每个连通区域内找到距离港口最近的购买点
	vector<int> nearestBuyPlace(connectedHarbors.size(), -1);
	for (int i = 0; i < connectedHarbors.size(); i++)
	{
		int minDistance = 10000;
		for (int j = 0; j < boatBuyPlace.size(); j++)
		{
			int distance = boatPathPlanner.getDistanceToHarbor(connectedHarbors[i][0], boatBuyPlace[j]);
			if (distance < minDistance)
			{
				minDistance = distance;
				nearestBuyPlace[i] = j;
			}
		}
	}

	//计算每个购买点的购买比例
	vector<double> boatBuyPlaceProportion(boatBuyPlace.size(), 0);
	for (int i = 0; i < boatBuyPlace.size(); i++)
	{
		int indexIn_nearestBuyPlace = find(nearestBuyPlace.begin(), nearestBuyPlace.end(), i) - nearestBuyPlace.begin();
		if (indexIn_nearestBuyPlace != nearestBuyPlace.size())
		{
			boatBuyPlaceProportion[i] = double(connectedHarbors[indexIn_nearestBuyPlace].size()) / harborNum;
		}
	}

	double expectedBoatNum = 1.0 / boatCapacity;
	//计算每个购买点的购买数量
	vector<int> boatBuyPlacePurchaseNum(boatBuyPlace.size(), 0);
	for (int i = 0; i < boatBuyPlace.size(); i++)
	{
		//cerr << boatBuyPlaceProportion[i] * expectedBoatNum << ' ';
		boatBuyPlacePurchaseNum[i] = ceil(boatBuyPlaceProportion[i] * expectedBoatNum);
		//cerr << "num";
		//cerr << boatBuyPlacePurchaseNum[i] << ' ';
	}

	vector<pair<int, int>> boatBuyPlacePurchaseNumAndIndex;
	for (int i = 0; i < boatBuyPlace.size(); i++)
	{
		boatBuyPlacePurchaseNumAndIndex.push_back(make_pair(boatBuyPlacePurchaseNum[i], i));
	}
	sort(boatBuyPlacePurchaseNumAndIndex.begin(), boatBuyPlacePurchaseNumAndIndex.end(), greater<pair<int, int>>());

	//生成一个购买数组
	vector<int>purchaseList;
	while (1)
	{
		int i = 0;
		for (i = 0; i < boatBuyPlacePurchaseNumAndIndex.size(); i++)
		{
			if (boatBuyPlacePurchaseNumAndIndex[i].first > 0)
			{
				purchaseList.push_back(boatBuyPlacePurchaseNumAndIndex[i].second);
				boatBuyPlacePurchaseNumAndIndex[i].first--;
			}
			else
				break;
		}
		if (i == 0)
			break;
	}

	return purchaseList;
}

