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
	// Ųһ�£���harborŲ��ǰ�룬-n������Ų������
	vector<Harbor> tmp=vector<Harbor>();
	tmp.insert(tmp.end(), harbors.begin(), harbors.begin() + (-sellPlace) - 1);
	harbors.erase(harbors.begin(), harbors.begin() + (-sellPlace) - 1);
	harbors.insert(harbors.end(), tmp.begin(), tmp.end());
	this->robots=vector<Robot>();
	this->boats=vector<Boat>();
	int Capacity = 0;
	scanf("%d", &Capacity);
	this->boatCapacity = Capacity;

	// ���ɴ���·��ͼ
	this->boatPathPlanner=BoatPathPlanner();
	this->boatPathPlanner.initBoatPathPlanner(map.point, harbors, boatBuyPlace, harborNum);

	// ��β
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
	if (!scanf("%d %d", &this->frame, &money)) // ������һ��frame��money
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
		if (nearestHarborId != -1) // ��Ʒ�пɴ�ۿ�
			harbors[nearestHarborId].appendProfitRate(newProduct.bestProfitRate);
	}
	for (int i = startProductId; i < products.size(); i++)
	{
		if (products[i].expireTime <= frame)
		{
			startProductId++;
			//������Ƿ��������Ĵ���
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
	scanf("%s", end); // ������β
	
	
	return true;
}

void Scheduler::findHarbor(int robotId)
{
	float minDistance = 10000;
	int bestHarborId = robots[robotId].goalHarbor;
	if(bestHarborId==-1 || harbors[bestHarborId].tiger) // ����Ѿ�ָ���ۿ����ٲ���
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
		if(robots[robotId].goalHarbor==-1) // ��ʼ�����ԭ��û��λ�ڸۿ�
			harbors[bestHarborId].appendRobot(robotId);
		else if (robots[robotId].goalHarbor != bestHarborId) // ������˵��������if��������findHarbor��Ŀ��ۿڷ����仯
		{
			harbors[robots[robotId].goalHarbor].removeRobot(robotId);
			harbors[bestHarborId].appendRobot(robotId);
		}
		robots[robotId].assignTask(pathPlanner.getPathToHarbor(bestHarborId, Coord(robots[robotId].x, robots[robotId].y)), 2, bestHarborId, bestHarborId);
	}

}

void Scheduler::findProductAndHarbor(int robotId)
{
	if (robots[robotId].moves.size() != robots[robotId].curMoveCount) // �������δ�����·��Ҫ����Ŀ��
		return;
	int startHarbor = robots[robotId].atHarbor; // ����ǰfindHarbor���õ�Ŀ��ۿ���Ϊ�����ۿ�

	int bestProductId;
	int bestHarborId = -1;
	double maxProfitRate = -1;
	for (int i=startProductId;i<products.size();i++)
	{
		if(products[i].locked) // ��Ʒ������
			continue;
		int nearestHarborId = products[i].getNearestHarborId();
		if (nearestHarborId == -1) // ��Ʒû�пɴ�ۿ�
			continue;
		int pathLen1 = products[i].distanceToHarbors[startHarbor]; 
		if (pathLen1 == -1) // ��Ʒ���ɵ�������˳����ۿ�
			continue;
		if(frame+pathLen1>=products[i].expireTime) // �����˵����Ʒ��ʱ�䳬���˹���ʱ��
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
		if (robots[robotId].goalHarbor != bestHarborId) // ������Ŀ��ۿڷ����仯
		{
			harbors[robots[robotId].goalHarbor].removeRobot(robotId);
			harbors[bestHarborId].appendRobot(robotId);
		}
		robots[robotId].assignTask(pathPlanner.getPathFromHarbor(startHarbor, Coord(products[bestProductId].x, products[bestProductId].y)), 0, -1, bestHarborId);
		products[bestProductId].locked = true;
		harbors[bestHarborId].removeProfitRate(products[bestProductId].bestProfitRate); // ��Ʒ�����󣬸ۿڶ�Ӧ�����ʼ���
		robots[robotId].carryProduct = bestProductId;
	}

}


void Scheduler::Update() {
	// �˴������ߣ������ָ��
	for (int i = 0; i <robotNum; i++) 
	{
		if (robots[i].target == -1) 
		{
			findProductAndHarbor(i); //Ϊ��i�������˷�������
		}
		else if (robots[i].target == 1)
		{
			findHarbor(i); // ��i��������ȥ����ĸۿ� 
		}
	}
	//����ָ����ƻ������ƶ�
	// ��ײͼ��ʼ��
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
		collisionMap[robots[i].x][robots[i].y] = i + 1; // ע��������i+1����Ϊ������id��0��ʼ
	}
	// �ƶ�ջ��ʣ��ջ
	stack<pair<int, int>> moveRobots;
	stack<pair<int, int>> restRobots;
	// ʣ��ջ��ʼ��
	for (int i = robotNum - 1; i >= 0; i--) 
	{
		restRobots.push(make_pair(i,-1)); // first�ǻ�����id��second�ǻ����˵��ƶ�����
	}
	// �ҵ�ÿ�������˵��޳�ͻ�ƶ�����
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
	//����ָ����ƻ������ƶ�
	while(!moveRobots.empty())
	{
		pair<int, int> curRobot = moveRobots.top();
		moveRobots.pop();
		int i= curRobot.first;
		int action = robots[i].command(curRobot.second);
		if (action == 1) //������Ʒ
		{
			harbors[robots[i].atHarbor].productPrices.push_back(products[robots[i].carryProduct].price);
			score += products[robots[i].carryProduct].price;
			// �ѷ�����Ʒ���Ըۿڽ���ͬ��
			// ���ۿڵĶ�������Ʒ����+1
			this->harborWhoGotReceive.push_back(robots[i].atHarbor);
			harbors[robots[i].atHarbor].totalReceived++;
		}
		
	}
	// �������ƶ�����
	// ���㶩��ͬ��
	for (int i = 0; i < harborWhoGotReceive.size(); i++) {
		int harborId=harborWhoGotReceive[i];
		int boatId = hasBoat(harborId);
		// ���д��ڸ�
		if (boatId!=-1) {
			// ����ǰ���Ķ���ͬ��
			boats[boatId].addOneOrder();
			// ���ۿڵĶ���ͬ��
			harbors[harborId].addOneOrder();
			harborWhoGotReceive.erase(harborWhoGotReceive.begin() + i);
			i--;
		}
	}
	if (harborWhoGotReceive.size() > 0) {
		// ������޴��ۿڽ��յ��»������ͬ��
		for (int i = 0; i < harborWhoGotReceive.size(); i++) {
			int harborId = harborWhoGotReceive[0];
			// ���Ȳ����Ƿ��д�Ҫ��
			if (harbors[harborId].orders.size() == 0) {
				// û�д�Ҫ����������
			}
			else {
				// �д�Ҫ�������������󵽲鿴�ܷ�Ӹ������Ĵ�
				for (int j = 0; j < harbors[harborId].orders.size(); j++) {
					int boatId = harbors[harborId].orders[j].id;
					if (boats[boatId].getOrderCapacity() + 1 < boatCapacity) {
						// �ŵ��£�ͬ��
						boats[boatId].addOneOrder();
						harbors[harborId].addOneOrder();
						harborWhoGotReceive.erase(harborWhoGotReceive.begin() + i);
						i--;
						break;
					}
					else {
						// �Ų��£�������
					}
				}
			}
		}
	}
	// ����ɸһ�¸ۿ�
	//for (int i = 0; i < 10; i++) {
	//	if (harbor[i].orders.size()==0 && harbor[i].productPrices.size() >= boatCapacity)
	//		harbor[i].tiger = true;
	//}

	// ���ȿ����Ƿ���Ҫ��ȥ����
	
		



	// ��β
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
	// countΪ���ڸۿ�װ�صĻ����ܼ�ֵ
	// �ڴ˴��ж�ʱ���Ƿ񻹹����ۿ�
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
* @param countNeeded ����װ�صĻ�������
* @param timeToComeBack ֱ�ӻ�ȥ������ʱ��
* @param holdedValue ��װ�Ļ����ֵ
*/
int Scheduler::selectFastestHarbor(int countNeeded, int timeTocomeBack, int holdedValue, int boatId) {
	int bestID = -1;
	float value = -1;
	for (int i = 0; i < 10; i++) {
		// ����δ���ļ�ֵ����λ��Ч�� Ǯ/ʱ�䣩
		float tmpValue = getFutureValue(i, countNeeded, boatId);
		// (�����뿼�ǣ����øۿڻ���ǳ��࣬�Ƿ�Ҫ�Ŷӣ��Ŷӿ���Ҳ�ǻ����)

		if (tmpValue > value) {
			value = tmpValue;
			bestID = i;
		}
	}
	
	return bestID;
}
/*
* @param harborId Ŀ��ۿ�id
* @param total    ����װ��������
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
	// countΪ���ڸۿ�װ�صĻ����ܼ�ֵ
	// �ڴ˴��ж�ʱ���Ƿ񻹹����ۿ�
	int loadcost = number / harbors[harborId].velocity;
	if (loadcost +  500 + frame >= 15000) {
		return -1;
	}
	// ��harbor���ṩ�Ļ�������Ӧ��Լ����countNeeded
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
	// �л��Ĳ�ͬ�ǣ���Ҫ���ԭ���Ķ���
	int prevHarbor = boats[boatId].originPos();
	// ��ԭ���ĸۿ�ͬ��
	harbors[prevHarbor].clearOneOrder();
	// ���µĸۿ�ͬ��
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