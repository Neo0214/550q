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
		Node** pathMap = this->map[i];
		//写到文件
		//if (i == 3) {
		//	ofstream out("pathMap.txt");
		//	for (int i = 0; i < LEN; i++) {
		//		for (int j = 0; j < LEN; j++) {
		//			if (pathMap[i][j].distance == -1)
		//				out << 'x';
		//			else
		//				out << char('0' + pathMap[i][j].direct);
		//
		//		}
		//		out << endl;
		//
		//
		//
		//	}
		//	out.close();
		//	exit(0);
		//
		//}


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
	pathMap[begin.x][begin.y].set(5, 0);
	if (begin.x + 1 < LEN && isBoatPath(map[begin.x + 1][begin.y])) {
		pathMap[begin.x + 1][begin.y].set(2, 1);
		q.push(Coord(begin.x + 1, begin.y));
	}
	if (begin.x - 1 >= 0 && isBoatPath(map[begin.x - 1][begin.y])) {
		pathMap[begin.x - 1][begin.y].set(3, 1);
		q.push(Coord(begin.x - 1, begin.y));
	}
	if (begin.y + 1 < LEN && isBoatPath(map[begin.x][begin.y + 1])) {
		pathMap[begin.x][begin.y + 1].set(1, 1);
		q.push(Coord(begin.x, begin.y + 1));
	}
	if (begin.y - 1 >= 0 && isBoatPath(map[begin.x][begin.y - 1])) {
		pathMap[begin.x][begin.y - 1].set(0, 1);
		q.push(Coord(begin.x, begin.y - 1));
	}
	while (!q.empty()) {
		Coord cur = q.front();
		int distance = pathMap[cur.x][cur.y].distance;
		q.pop();
		for (int i = 0; i < 4; i++) {
			Coord next = cur + i;
			if (next.x < 0 || next.y < 0 || next.x >= LEN || next.y >= LEN) {
				continue;
			}
			if ((pathMap[next.x][next.y].direct <= -1) && isBoatPath(map[next.x][next.y])) {
				// 已过基础判断，还需要验证方向下船体是否都可以用
				int type = wholeBoatAvailable(map, next, reverseMove[i]);
				if (type == 1) {
					// 全部条件通过，且船身全部在正常范围内
					pathMap[next.x][next.y].set(reverseMove[i], distance + 1);
					q.push(next);
				}
				else if (type == 2) {
					// 全部条件通过，但是船身有一部分在边界上
					pathMap[next.x][next.y].set(reverseMove[i] - 5, distance + 1); // 边界标记是不能作为探测点的
					q.push(next);
				}
			}
		}

	}


}

int BoatPathPlanner::wholeBoatAvailable(char map[LEN][LEN], Coord& checkPos, int direct)
{
	int startX = checkPos.x, startY = checkPos.y;
	int endX = checkPos.x, endY = checkPos.y;
	switch (direct) {
	case 0:
		// 向右
		endX += 1;
		startY -= 1;//
		endY += 1;
		break;
	case 1:
		// 向左
		startX -= 1;
		startY -= 1;
		endY += 1;//
		break;
	case 2:
		// 向上
		startX -= 1;
		endX += 1; //
		endY += 1;
		break;
	case 3:
		// 向下
		startX -= 1;//
		endX += 1;
		startY -= 1;
		break;
	}
	if (startX < 0 || startY < 0 || endX >= LEN || endY >= LEN) {
		return 0;
	}
	bool flag = true;
	for (int i = startX; i <= endX; i++) {
		for (int j = startY; j <= endY; j++) {
			if (!isBoatPath(map[i][j])) {
				flag = false;
			}
		}
	}
	if (flag == true) {
		return 1;
	}
	else {
		int startX = checkPos.x, startY = checkPos.y;
		int endX = checkPos.x, endY = checkPos.y;
		switch (direct) {
		case 0:
			// 向右
			endX += 1;
			endY += 1;
			break;
		case 1:
			// 向左
			startX -= 1;
			startY -= 1;
			break;
		case 2:
			// 向上
			startX -= 1;
			endY += 1;
			break;
		case 3:
			// 向下
			endX += 1;
			startY -= 1;
			break;
		}
		if (startX < 0 || startY < 0 || endX >= LEN || endY >= LEN) {
			return 0;
		}
		for (int i = startX; i <= endX; i++) {
			for (int j = startY; j <= endY; j++) {
				if (!isBoatPath(map[i][j])) {
					return 0;
				}
			}
		}
		return 2;
	}
	return 0;
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

int BoatPathPlanner::nextMove(Coord curPos, int curDirect, int mapId, char originMap[LEN][LEN])
{
	Node** pathMap = this->map[mapId]; // 调出地图
	int curDistance = pathMap[curPos.x][curPos.y].distance;
	if (pathMap[curPos.x][curPos.y].direct == 5) {
		// 已走到关键点
		return FORWARD;
	}

	switch (curDirect) {
	case 0:
		// 当前朝右
		if (pathMap[curPos.x][curPos.y].direct == 2) {
			return LEFTTURN;
		}
		else if ((pathMap[curPos.x + 1][curPos.y + 1].direct == 3 && pathMap[curPos.x + 1][curPos.y + 1].distance < curDistance) || pathMap[curPos.x + 1][curPos.y + 1].direct == ORIGIN) {
			return RIGHTTURN;
		}
		break;
	case 1:
		// 当前朝左
		if (pathMap[curPos.x][curPos.y].direct == 3) {
			return LEFTTURN;
		}
		else if ((pathMap[curPos.x - 1][curPos.y - 1].direct == 2 && pathMap[curPos.x - 1][curPos.y - 1].distance < curDistance) || pathMap[curPos.x - 1][curPos.y - 1].direct == ORIGIN) {
			return RIGHTTURN;
		}
		break;
	case 2:
		// 当前朝上
		if (pathMap[curPos.x][curPos.y].direct == 1) {
			return LEFTTURN;
		}
		else if ((pathMap[curPos.x - 1][curPos.y + 1].direct == 0 && pathMap[curPos.x - 1][curPos.y + 1].distance < curDistance) || pathMap[curPos.x - 1][curPos.y + 1].direct == ORIGIN) {
			return RIGHTTURN;
		}
		break;
	case 3:
		// 当前朝下
		if (pathMap[curPos.x][curPos.y].direct == 0) {
			return LEFTTURN;
		}
		else if ((pathMap[curPos.x + 1][curPos.y - 1].direct == 1 && pathMap[curPos.x + 1][curPos.y - 1].distance < curDistance) || pathMap[curPos.x + 1][curPos.y - 1].direct == ORIGIN) {
			return RIGHTTURN;
		}
		break;

	}
	if (curDirect == pathMap[curPos.x][curPos.y].direct || canForward(curDirect, curPos, originMap))
		return FORWARD;
	else {
		// 实在是没有可走的
		return hasToChooseTurn(curDirect, curPos, originMap);
	}
	return -1;
}

int BoatPathPlanner::hasToChooseTurn(int curDirect, Coord curPos, char originMap[LEN][LEN])
{
	int startX = curPos.x, startY = curPos.y;
	int endX = curPos.x, endY = curPos.y;
	switch (curDirect) {
	case 0:
		startX -= 1;
		endX += 1;
		endY += 1;
		break;
	case 1:
		startX -= 1;
		endX += 1;
		startY -= 1;
		break;
	case 2:
		startX -= 1;
		startY -= 1;
		endY += 1;
		break;
	case 3:
		endX += 1;
		startY -= 1;
		endY += 1;
		break;
	}
	// 先尝试左转方案
	for (int i = startX; i <= endX; i++) {
		for (int j = startY; j <= endY; j++) {
			if (i < 0 || j < 0 || i >= LEN || j >= LEN || !isBoatPath(originMap[i][j])) {
				return RIGHTTURN;
			}
		}
	}
	return LEFTTURN;

}


bool BoatPathPlanner::canForward(int curDirect, Coord curPos, char originMap[LEN][LEN])
{
	int startX = curPos.x, startY = curPos.y;
	int endX = curPos.x, endY = curPos.y;
	switch (curDirect) {
	case 0:
		// 向右挪动
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
	}
	for (int i = startX; i <= endX; i++) {
		for (int j = startY; j <= endY; j++) {
			if (i < 0 || j < 0 || i >= LEN || j >= LEN || !isBoatPath(originMap[i][j])) {
				cerr << "can't forward" << '\n';
				return false;
			}
		}
	}
	return true;
}

int BoatPathPlanner::getDistance(Coord curPos, int mapId)
{
	return map[mapId][curPos.x][curPos.y].distance;
}