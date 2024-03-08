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
				this->robot[robotCount++]=Robot(i,j);
			}
			else if (line[j] == 'B') {
				map.setPoint(i, j, HARBOR);
			}
		}
		cerr << i << endl;
	}
	// 10�иۿ�
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


	//��ʼ��·���滮��

	pathPlanner.initHarborPath(map.point,harborsCoord);

	//test
	auto moves = pathPlanner.getPathToHarbor(0, Coord(100, 100));
	for (auto move : moves)
	{
		cerr<<move<<endl;
	}


	// ��β
	char end[100];
	scanf("%s",end);
	printf("OK\n");
	fflush(stdout);
}

bool Scheduler::NextFrame() {
	int money;
	if (!scanf("%d %d", &this->frame, &money)) // ������һ��frame��money
		return false;
	int newGoodsCount;
	scanf("%d", &newGoodsCount);
	for (int i = 0; i < newGoodsCount; i++) {
		int x, y, val;
		scanf("%d %d %d", &x, &y, &val);
		map.setGoods(x, y, val, frame + 1000);
		int distanceToHarbors[HARBOR_NUM];
		for (int i = 0; i < HARBOR_NUM; i++)
		{
			distanceToHarbors[i]=pathPlanner.getPathToHarbor(i,Coord(x,y)).size(); //ʱ���д��Ż�
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
	scanf("%s", end); // ������β
	return true;
}


void Scheduler::writeCommand() {
	// ���ָ��
}

void Scheduler::RobotCommand(string command, int id, int direct)
{
	if (direct == -1) {
		printf("%s %d\n", command.c_str(), id);
		return;
	}
	printf("%s %d %d\n", command.c_str(), id, direct);
}

void Scheduler::findProductAndHarbor(int robotId)
{
	int startHarbor = robot[robotId].atHarbor;
	Product bestProduct;
	int bestHarborId =-1;
	double maxProfitRate = 0;
	for (Product product : products)
	{
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
	// �˴������ߣ������ָ��
	for (int i = 0; i < 10; i++) 
	{
		if (robot[i].target == -1) 
		{
			// ������û��Ŀ�꣬��Ҫ����
			if (robot[i].atHarbor == -1)
			{
				cerr << "��Ŀ��Ļ����˲��ڸۿ�" << endl;
				break; // TODO�����ڸۿڵĻ����˵��������
			}
			findProductAndHarbor(i); //Ϊ��i�������˷�������
		}
	}



	// ��β
	printf("OK\n");
	fflush(stdout);
}