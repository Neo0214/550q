#include "BoatPathPlanner.h"

BoatPathPlanner::BoatPathPlanner(int harborNum, int sellPlaceNum) {
	this->harborNum = harborNum;
	this->sellPlaceNum = sellPlaceNum;

}
BoatPathPlanner::~BoatPathPlanner() {
	//for (int i = 0; i < harborNum + sellPlaceNum; i++) {
	//	for (int j = 0; j < LEN; j++) {
	//		delete[] this->map[i][j];
	//	}
	//	delete[] this->map[i];
	//}
	//delete[] this->map;
}
void BoatPathPlanner::clean() {
	for (int i = 0; i < harborNum + sellPlaceNum; i++) {
		for (int j = 0; j < LEN; j++) {
			delete[] this->map[i][j];
		}
		delete[] this->map[i];
	}
	delete[] this->map;
}

void BoatPathPlanner::init(char map[LEN][LEN], vector<Harbor>& harbors, vector<Delivery>& deliveries) {
	this->map = new Node * *[harborNum + sellPlaceNum];
	for (int i = 0; i < harborNum + sellPlaceNum; i++) {
		this->map[i] = new Node * [LEN];
		for (int j = 0; j < LEN; j++) {
			this->map[i][j] = new Node[LEN];
		}
	}
	for (int i = 0; i < harborNum; i++) {
		// 对每个港口生成前往路径地图
		BFSearch(this->map[i], map, harbors[i].berthCoord);
	}
	for (int i = harborNum; i < harborNum + sellPlaceNum; i++) {
		// 对每个售卖点生成前往路径地图
		BFSearch(this->map[i], map, deliveries[i - harborNum].getPos());
	}
}
void BoatPathPlanner::BFSearch(Node** pathMap, char map[LEN][LEN], Coord begin)
{
	queue<Coord> q;
	q.push(begin);
	pathMap[begin.x][begin.y].set(0, 0);
	while (!q.empty()) {
		Coord cur = q.front();
		int distance = pathMap[cur.x][cur.y].distance;
		q.pop();
		for (int i = 0; i < 4; i++) {
			Coord next = cur + i;
			if (pathMap[next.x][next.y].direct == -1 && isBoatPath(map[next.x][next.y])) {
				// 已过基础判断，还需要验证方向下船体是否都可以用
				if (wholeBoatAvailable(map, next, reverseMove[i])) {
					// 全部条件通过
					pathMap[next.x][next.y].set(reverseMove[i], distance + 1);

					q.push(next);
				}
			}
		}

	}
	// 写到文件
	//ofstream out("pathMap.txt");
	//for (int i = 0; i < LEN; i++) {
	//	for (int j = 0; j < LEN; j++) {
	//		if (pathMap[i][j].distance == -1)
	//			out << 'x';
	//		else
	//			out << pathMap[i][j].distance;
	//	}
	//	out << endl;
	//}
	//out.close();
	//exit(0);

}

bool BoatPathPlanner::wholeBoatAvailable(char map[LEN][LEN], Coord& checkPos, int direct)
{
	int startX = checkPos.x, startY = checkPos.y;
	int endX = checkPos.x, endY = checkPos.y;
	switch (direct) {
	case 0:
		// 向右
		endX += 1;
		endY += 2;
		break;
	case 1:
		// 向左
		startX -= 1;
		startY -= 2;
		break;
	case 2:
		// 向上
		startX -= 2;
		endY += 1;
		break;
	case 3:
		// 向下
		endX += 2;
		startY -= 1;
		break;
	}
	if (startX < 0 || startY < 0 || endX >= LEN || endY >= LEN) {
		return false;
	}
	for (int i = startX; i <= endX; i++) {
		for (int j = startY; j <= endY; j++) {
			if (!isBoatPath(map[i][j])) {
				return false;
			}
		}
	}
	return true;
}

// 从当前位置找到最近的售卖点
int BoatPathPlanner::findBestSellPlace(Coord cur)
{
	int minDistance = INT_MAX;
	int minIndex = -1;
	for (int i = harborNum; i < harborNum + sellPlaceNum; i++)
	{
		if (this->map[i][cur.x][cur.y].distance < minDistance) {
			minDistance = this->map[i][cur.x][cur.y].distance;
			minIndex = i;
		}
	}
	return minIndex;
}

int BoatPathPlanner::nextMove(Coord curPos, int curDirect, int mapId)
{
	Node** pathMap = this->map[mapId]; // 调出地图
	return 0;
}