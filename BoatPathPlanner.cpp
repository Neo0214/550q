#include "BoatPathPlanner.h"

BoatPathPlanner::BoatPathPlanner(int harborNum, int sellPlaceNum) {
	this->harborNum = harborNum;
	this->sellPlaceNum = sellPlaceNum;
	this->crossedDistance = vector<vector<int>>(harborNum + sellPlaceNum, vector<int>(harborNum + sellPlaceNum, 10000));
	this->crossedTarget = vector<vector<int>>(harborNum + sellPlaceNum, vector<int>(harborNum + sellPlaceNum, -1));
}
BoatPathPlanner::~BoatPathPlanner() {
	// 释放内存
	//for (int i = 0; i < harborNum + sellPlaceNum; i++) {
	//	for (int j = 0; j < LEN; j++) {
	//		for (int k = 0; k < LEN; k++) {
	//			delete[] this->map[i][j][k];
	//		}
	//		delete[] this->map[i][j];
	//	}
	//	delete[] this->map[i];
	//}
	//delete[] this->map;
}
void BoatPathPlanner::clean() {
	for (int i = 0; i < harborNum + sellPlaceNum; i++) {
		for (int j = 0; j < LEN; j++) {
			for (int k = 0; k < LEN; k++) {
				delete[] this->map[i][j][k];
			}
			delete[] this->map[i][j];
		}
		delete[] this->map[i];
	}
	delete[] this->map;

}

void BoatPathPlanner::searchAllPath(const char my_map[LEN][LEN], vector<Coord> startCoords, Node*** path) // path是得到的路径矩阵
{
	queue<BoatState> q;
	bool visited[LEN][LEN][4] = { 0 };
	for (auto startCoord : startCoords)
	{

		for (int i = 0; i < 4; i++)
		{
			if (isLegalWholeBoat(BoatState(startCoord, i), my_map) && !visited[startCoord.x][startCoord.y][i]) {
				q.push(BoatState(startCoord, i));
				path[startCoord.x][startCoord.y][i].distance = 0;
				visited[startCoord.x][startCoord.y][i] = true;
			}
		}

	}

	while (!q.empty())
	{
		BoatState p = q.front();
		q.pop();
		//右左上下

		// 随机构建一个0到3的数组
		int a[3] = { 0,1,2 };
		// 随机打乱数组
		for (int i = 0; i < 3; i++)
		{
			int j = rand() % 3;
			swap(a[i], a[j]);
		}

		for (int idx = 0; idx < 3; idx++)
		{
			int i = a[idx];
			BoatState neighbor = p - i;

			if (isLegalWholeBoat(neighbor, my_map) && !visited[neighbor.x][neighbor.y][neighbor.direction])
			{

				q.push(neighbor);
				visited[neighbor.x][neighbor.y][neighbor.direction] = true;
				path[neighbor.x][neighbor.y][neighbor.direction].move = i;
				path[neighbor.x][neighbor.y][neighbor.direction].distance = path[p.x][p.y][p.direction].distance + 1;
			}
		}
	}


}

void BoatPathPlanner::init(char my_map[LEN][LEN], vector<Harbor>& harbors, vector<Delivery>& deliveries) {
	this->map = new Node * **[harborNum + sellPlaceNum];
	for (int i = 0; i < harborNum + sellPlaceNum; i++) {
		this->map[i] = new Node * *[LEN];
		for (int j = 0; j < LEN; j++) {
			this->map[i][j] = new Node * [LEN];
			for (int k = 0; k < LEN; k++) {
				this->map[i][j][k] = new Node[4];
			}
		}
	}

	for (int i = 0; i < harborNum; i++) {
		// 对每个售卖点生成前往路径地图
		vector<Coord> startCoord = harbors[i].getCoord();
		searchAllPath(my_map, startCoord, map[i]);
	}
	for (int i = harborNum; i < harborNum + sellPlaceNum; i++) {
		vector<Coord> startCoord = { deliveries[i - harborNum].getPos() };
		searchAllPath(my_map, startCoord, map[i]);
	}
	// 构建交叉路径距离图
	for (int i = 0; i < harborNum + sellPlaceNum; i++) {
		for (int j = 0; j < harborNum + sellPlaceNum; j++) {
			if (i == j) {
				crossedDistance[i][j] = 0;
			}
			else {
				if (i < harborNum) {
					// 从港口
					for (int k = 0; k < 4; k++) {
						if (getDistance(BoatState(harbors[i].getCoord()[0], k), j) != -1) {
							crossedDistance[i][j] = getDistance(BoatState(harbors[i].getCoord()[0], k), j);
							break;
						}
					}
				}
				else {
					// 从售卖点
					for (int k = 0; k < 4; k++) {
						if (getDistance(BoatState(deliveries[i - harborNum].getPos(), k), j) != -1) {
							crossedDistance[i][j] = getDistance(BoatState(deliveries[i - harborNum].getPos(), k), j);
							break;
						}
					}
				}
			}
		}
	}
	// 距离图已生成，接下来转换成最短路径指向
	for (int i = 0; i < harborNum + sellPlaceNum; i++) {
		for (int j = 0; j < harborNum + sellPlaceNum; j++) {
			if (i == j) {
				continue;
			}
			int minDistance = crossedDistance[i][j];
			int minTarget = j;
			for (int k = 0; k < harborNum + sellPlaceNum; k++) {
				if (k == i) {
					continue;
				}
				if (crossedDistance[i][k] + crossedDistance[k][j] < minDistance * 1.1) {
					minDistance = crossedDistance[i][k] + crossedDistance[k][j];
					minTarget = k;
				}
			}
			crossedDistance[i][j] = minDistance;
			crossedTarget[i][j] = minTarget;
		}

	}
	for (int i = 0; i < harborNum + sellPlaceNum; i++)
	{
		for (int j = 0; j < harborNum + sellPlaceNum; j++)
		{
			cerr << crossedTarget[i][j] << " ";
		}
		cerr << endl;
	}
}
int BoatPathPlanner::getNextId(int curId, int index)
{
	//cerr << curId << " " << index << endl;
	return crossedTarget[curId][index];
}
int BoatPathPlanner::getCrossedDistance(int curId, int index) {
	return crossedDistance[curId][index];
}

bool BoatPathPlanner::isLegalWholeBoat(BoatState neighbor, const char my_map[LEN][LEN])
{
	int startX = neighbor.x, startY = neighbor.y;
	int endX = neighbor.x, endY = neighbor.y;
	int direction = neighbor.direction;
	switch (direction) {
	case 0:
		endX += 1;
		endY += 2;
		break;
	case 1:
		startX -= 1;
		startY -= 2;
		break;
	case 2:
		startX -= 2;
		endY += 1;
		break;
	case 3:
		endX += 2;
		startY -= 1;
		break;
	}
	if (startX < 0 || startY < 0 || endX >= LEN || endY >= LEN)
		return false;
	for (int i = startX; i <= endX; i++) {
		for (int j = startY; j <= endY; j++) {
			if (!isBoatPath(my_map[i][j])) {
				return false;
			}
		}
	}
	return true;
}

vector<int> BoatPathPlanner::getPath(BoatState start, int targetId)
{
	Node*** path = this->map[targetId]; // 调出地图
	vector<int> res = vector<int>(); // 动作序列
	BoatState cur = start;
	while (path[cur.x][cur.y][cur.direction].distance != 0) {
		// 未到终点，行走当前行动
		res.push_back(path[cur.x][cur.y][cur.direction].move);
		//cerr << cur.x << " " << cur.y << " " << cur.direction << endl;
		//cerr << path[cur.x][cur.y][cur.direction].move << endl;
		//cerr << path[cur.x][cur.y][cur.direction].distance << endl;
		// 更新行动后位置
		update(cur, path[cur.x][cur.y][cur.direction].move);
	}
	return res;
}

void BoatPathPlanner::update(BoatState& cur, short move)
{
	switch (cur.direction) {
	case 0:
		switch (move)
		{
		case LEFTTURN:
			cur.x += 1;
			cur.y += 1;
			cur.direction = 2;
			break;
		case FORWARD:
			cur.y += 1;
			break;
		case RIGHTTURN:
			cur.y += 2;
			cur.direction = 3;
			break;
		}
		break;
	case 1:
		switch (move) {
		case LEFTTURN:
			cur.x -= 1;
			cur.y -= 1;
			cur.direction = 3;
			break;
		case FORWARD:
			cur.y -= 1;
			break;
		case RIGHTTURN:
			cur.y -= 2;
			cur.direction = 2;
			break;
		}
		break;
	case 2:
		switch (move) {
		case LEFTTURN:
			cur.x -= 1;
			cur.y += 1;
			cur.direction = 1;
			break;
		case FORWARD:
			cur.x -= 1;
			break;
		case RIGHTTURN:
			cur.x -= 2;
			cur.direction = 0;
			break;
		}
		break;
	case 3:
		switch (move) {
		case LEFTTURN:
			cur.x += 1;
			cur.y -= 1;
			cur.direction = 0;
			break;
		case FORWARD:
			cur.x += 1;
			break;
		case RIGHTTURN:
			cur.x += 2;
			cur.direction = 1;
			break;
		}
		break;
	}

}

int BoatPathPlanner::getDistance(BoatState cur, int targetId)
{
	Node*** path = this->map[targetId];
	return path[cur.x][cur.y][cur.direction].distance;
}