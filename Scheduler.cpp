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
				map.setPoint(i, j, EMPTY); // ֱ�Ӱ��յش���
				this->robot[robotCount]=Robot(robotCount,i,j);
				robotCount++;
			}
			else if (line[j] == 'B') {
				map.setPoint(i, j, HARBOR);
			}
		}
	}
	// 10�иۿ�
	int x, y, id, time, velocity;
	Coord harborsCoord[10];
	for (int i = 0; i < 10; i++) {
		scanf("%d %d %d %d %d", &id, &x, &y, &time, &velocity);
		harbor[i] = Harbor(id, x, y, time, velocity);
		harbor[i].getBestCoord(map.point);
		harborsCoord[i] = Coord(harbor[i].x,harbor[i].y);
	}
	// �Ըۿ�����ʼ������
	//initHarbor();
	//
	int capacity;
	scanf("%d", &capacity);
	for (int i = 0; i < 5; i++) {
		boat[i] = Boat(i, capacity);
	}
	this->boatCapacity = capacity;
	//��ʼ��·���滮��
	pathPlanner.initHarborPath(map.point,harborsCoord);

	int minTime= INT_MAX;
	for (int i = 0; i < 10; i++) {
		if (harbor[i].time < minTime) {
			minTime = harbor[i].time;
		}
	}
	minBackTime = minTime;

	// ��ʼ���ۿڸ�����
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

	// ��β
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
	if (!scanf("%d %d", &this->frame, &money)) // ������һ��frame��money
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
	for (int i = startProductId; i < products.size(); i++)
	{
		if (products[i].expireTime <= frame)
			startProductId++;
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
	scanf("%s", end); // ������β
	
	
	return true;
}

void Scheduler::findHarbor(int robotId)
{
	float minDistance = 10000;
	int bestHarborId = -1;
	for (int i = 0; i < HARBOR_NUM; i++)
	{
		float distance = pathPlanner.getDistanceToHarbor(i, Coord(robot[robotId].x, robot[robotId].y))/float((harbor[i].endingCost)+harbor[i].area);
		if (distance >0 && distance < minDistance)
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
	if (robot[robotId].moves.size() != robot[robotId].curMoveCount) // �������δ�����·��Ҫ����Ŀ��
		return;
	int startHarbor = robot[robotId].atHarbor; // ����ǰfindHarbor���õ�Ŀ��ۿ���Ϊ�����ۿ�

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
		double profitRate = pow(products[i].price, 1 + valueImportance * robotId) / pathLen * exp(-expireTimeImportance * (products[i].expireTime - frame)) + ((pathLen1 == pathLen2) ? sameHarborBonus : 0); // 1.0 / ((pathLen1 - pathLen2)*(pathLen1 - pathLen2) + 0.5);
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


void Scheduler::Update() {
	// �˴������ߣ������ָ��
	for (int i = 0; i <ROBOT_NUM; i++) 
	{
		if (robot[i].target == -1) 
		{
			findProductAndHarbor(i); //Ϊ��i�������˷�������
		}
		else if (robot[i].target == 1)
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
	for (int i = 0; i < ROBOT_NUM; i++)
	{
		collisionMap[robot[i].x][robot[i].y] = i + 1; // ע��������i+1����Ϊ������id��0��ʼ
	}
	// �ƶ�ջ��ʣ��ջ
	stack<pair<int, int>> moveRobots;
	stack<pair<int, int>> restRobots;
	// ʣ��ջ��ʼ��
	for (int i = ROBOT_NUM - 1; i >= 0; i--) 
	{
		restRobots.push(make_pair(i,-1)); // first�ǻ�����id��second�ǻ����˵��ƶ�����
	}
	// �ҵ�ÿ�������˵��޳�ͻ�ƶ�����
	int countToughTask = 0;
	while(!restRobots.empty())
	{
		pair<int,int> curRobot = restRobots.top();
		restRobots.pop();
		
		pair<int, vector<int>> moveAndCollision = robot[curRobot.first].moveOneStep(collisionMap);
		if(countToughTask<=10)
			while (moveAndCollision.second.size())
			{
				// cerr << "tough task" << moveAndCollision.second.size() <<' ' << moveRobots.size() <<endl;
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
				pair<int,int> redoRobot = moveRobots.top();
				moveRobots.pop();
				robot[redoRobot.first].redoOneStep(collisionMap,redoRobot.second);
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
		int action = robot[i].command(curRobot.second);
		if (action == 1) //������Ʒ
		{
			harbor[robot[i].atHarbor].productPrices.push_back(products[robot[i].carryProduct].price);
			score += products[robot[i].carryProduct].price;
			// �ѷ�����Ʒ���Ըۿڽ���ͬ��
			// ���ۿڵĶ�������Ʒ����+1
			this->harborWhoGotReceive.push_back(robot[i].atHarbor);

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
			boat[boatId].addOneOrder();
			// ���ۿڵĶ���ͬ��
			harbor[harborId].addOneOrder();
			harborWhoGotReceive.erase(harborWhoGotReceive.begin() + i);
			i--;
		}
	}
	if (harborWhoGotReceive.size() > 0) {
		// ������޴��ۿڽ��յ��»������ͬ��
		for (int i = 0; i < harborWhoGotReceive.size(); i++) {
			int harborId = harborWhoGotReceive[0];
			// ���Ȳ����Ƿ��д�Ҫ��
			if (harbor[harborId].orders.size() == 0) {
				// û�д�Ҫ����������
			}
			else {
				// �д�Ҫ�������������󵽲鿴�ܷ�Ӹ������Ĵ�
				for (int j = 0; j < harbor[harborId].orders.size(); j++) {
					int boatId = harbor[harborId].orders[j].id;
					if (boat[boatId].getOrderCapacity() + 1 < boatCapacity) {
						// �ŵ��£�ͬ��
						boat[boatId].addOneOrder();
						harbor[harborId].addOneOrder();
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

	// �����ǻ����˵��ƶ��������Ǵ����ƶ�
	if (frame == 1) {
		for (int i = 0; i < 5; i++) {
			boat[i].gotoHarbor(i);
			// ����ʱ���Զ�������ͬ��
			synchronizeWhenGoOut(i, i);
		}
		
	}
	// ���ȿ����Ƿ���Ҫ��ȥ����
	for (int i = 0; i < 5; i++) {
		// ��ǡ��װ������ʱ�䲻��ʱ�����̻�ȥ����
		if (boat[i].getCurCapacity() == boatCapacity
			|| (boat[i].getPos() != -1 && boat[i].getStatus() == NORMAL && 15000 - frame < harbor[boat[i].getPos()].time)
			) {
			// ��������ʱ���Զ�������ͬ��
			clearWhenBoatComeBack(i, boat[i].getPos());
			boat[i].comeBack();
			
		}
	}
	// Ȼ�󿴴��Ƿ���Ҫ��һ���ۿ�
	for (int i = 0; i < 5; i++) {
		// ����ͣ����ĳ���ۿڣ��Ҹøۿ�ǡ��û�жѻ��Ļ�����Ը�װ�أ�Ѱ�ҿɻ��ĸۿڣ��˴���Ҫ���ǻ��ۿ��Ƿ��㣬��Ҫȥ̫�ٶѻ��ĵط���
		if (boat[i].getPos() != -1 && boat[i].getStatus() == NORMAL && boat[i].getPre() == 0 && harbor[boat[i].getPos()].productPrices.size()==0) {
			int harborId = selectFastestHarbor(boatCapacity-boat[i].getCurCapacity(),harbor[boat[i].getPos()].time,boat[i].getCurValue());
			// ���пɻ��ģ����̹�ȥ
			if (harborId != -1) {
				// �Ʋ��Ƿ������һ��
				if (frame + 500 + 500 + minBackTime > 15000) {
					// һ�������һ����
					harbor[harborId].endingCost = 20;
					cerr << "ok" << endl;
				}
				boat[i].gotoHarbor(harborId);
				// �������ۿ�ʱ���Զ�������ͬ��
				synchronizeWhenSwitch(i, harborId);
			}
			// �����-1��˵��û�к��ʵĸۿڣ����̻�ȥ����
			else {
				// ��������ʱ���Զ�������ͬ��
				clearWhenBoatComeBack(i, boat[i].getPos());
				boat[i].comeBack();
				
			}
		}
	}
	vector<int> freeBoat = getFreeBoat();
	// ���ݿ���������ѡ�����ĸۿ�
	for (int i = 0; i < freeBoat.size(); i++) {
		int harborId = selectAvailableFastestHarborWithGoingFromOriginPoint();
		// Ҫע��˴�boat�ӽ����ص������Ҫ�����Ƿ���ʱ���������
		if (harborId != -1) {
			if (frame + harbor[harborId].time + minBackTime*2> 15000) {
				harbor[harborId].endingCost = 20;
				cerr << "ok"<<endl;
			}
			boat[freeBoat[i]].gotoHarbor(harborId);
			// ����ʱ���Զ�������ͬ��
			synchronizeWhenGoOut(freeBoat[i], harborId);
			
		}
	}
	// ������Ʒ�Ժ󣬲���װ�ϴ�
		// �������������ϴ�������
		// ����ڸۿڣ���������ϴ�
	for (int i = 0; i < 5; i++)
	{
		if (boat[i].getPos() != -1 && boat[i].getStatus() == NORMAL) {
			loadGoods(&boat[i], &harbor[boat[i].getPos()]);
		}
	}
		



	// ��β
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
	// countΪ���ڸۿ�װ�صĻ����ܼ�ֵ
	// �ڴ˴��ж�ʱ���Ƿ񻹹����ۿ�
	int loadcost = (number) / harbor[harborId].velocity;
	if (loadcost + harbor[harborId].time*2 + frame >= 15000) {
		return -1;
	}
	return count/float(loadcost+harbor[harborId].time*2);
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
* @param countNeeded ����װ�صĻ�������
* @param timeToComeBack ֱ�ӻ�ȥ������ʱ��
* @param holdedValue ��װ�Ļ����ֵ
*/
int Scheduler::selectFastestHarbor(int countNeeded, int timeTocomeBack, int holdedValue) {
	int bestID = -1;
	float value = -1;
	for (int i = 0; i < 10; i++) {
		// ����δ���ļ�ֵ����λ��Ч�� Ǯ/ʱ�䣩
		float tmpValue = getFutureValue(i, countNeeded);
		// (�����뿼�ǣ����øۿڻ���ǳ��࣬�Ƿ�Ҫ�Ŷӣ��Ŷӿ���Ҳ�ǻ����)

		if (tmpValue > value) {
			value = tmpValue;
			bestID = i;
		}
	}
	if (/*countNeeded>10 || */(bestID != -1 && value>holdedValue / float(timeTocomeBack * 2))) {
		return bestID;
	}
	return -1;
}
/*
* @param harborId Ŀ��ۿ�id
* @param total    ����װ��������
*/
float Scheduler::getFutureValue(int harborId, int total) {
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
	// countΪ���ڸۿ�װ�صĻ����ܼ�ֵ
	// �ڴ˴��ж�ʱ���Ƿ񻹹����ۿ�
	int loadcost = number / harbor[harborId].velocity;
	if (loadcost + harbor[harborId].time + 500 + frame >= 15000) {
		return -1;
	}
	return count / float(loadcost + 500+harbor[harborId].time);
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
		//cerr<<"harbor "<<i<<" "<<count<<endl;
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
	// �л��Ĳ�ͬ�ǣ���Ҫ���ԭ���Ķ���
	int prevHarbor = boat[boatId].originPos();
	// ��ԭ���ĸۿ�ͬ��
	harbor[prevHarbor].clearOneOrder();
	// ���µĸۿ�ͬ��
	int goodsLeft = harbor[harborId].productPrices.size();
	for (int i = 0; i < harbor[harborId].orders.size(); i++) {
		goodsLeft -= harbor[harborId].orders[i].productNumber;
	}
	boat[boatId].clearOneOrder();
	boat[boatId].newOrder(goodsLeft, harborId);
	harbor[harborId].newOrder(goodsLeft, boatId);

}