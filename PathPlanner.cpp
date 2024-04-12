#include "PathPlanner.h"

void PathPlanner::searchAllPath(const char my_map[LEN][LEN], Coord startCoord, Path** path) // path是得到的路径矩阵
{
	queue<Coord> q;
	bool visited[LEN][LEN] = { { 0 } };
	q.push(startCoord);
	path[startCoord.x][startCoord.y].distance = 0;
	visited[startCoord.x][startCoord.y] = true;

	while (!q.empty())
	{
		Coord p = q.front();
		q.pop();
		//右左上下
		Coord neighbors[4] = { Coord(p.x,p.y + 1),Coord(p.x,p.y - 1),Coord(p.x - 1,p.y),Coord(p.x + 1,p.y) };

		// 随机构建一个0到3的数组
		int a[4] = { 0,1,2,3 };
		// 随机打乱数组
		for (int i = 0; i < 4; i++)
		{
			int j = rand() % 4;
			swap(a[i], a[j]);
		}

		for (int idx = 0; idx < 4; idx++)
		{
			int i = a[idx];
			Coord neighbor = neighbors[i];
			if (neighbor.x >= 0 && neighbor.x < LEN && neighbor.y >= 0 && neighbor.y < LEN
				&& isRobotPath(my_map[neighbor.x][neighbor.y]) && !visited[neighbor.x][neighbor.y])
			{
				q.push(neighbor);
				visited[neighbor.x][neighbor.y] = true;
				path[neighbor.x][neighbor.y].lastCoord = p;
				path[neighbor.x][neighbor.y].move = i;
				path[neighbor.x][neighbor.y].distance = path[p.x][p.y].distance + 1;
			}
		}
	}


}



void PathPlanner::initHarborPath(const char my_map[LEN][LEN], vector<Coord> harborCoords)
{
	// 为harborsPaths分配内存为harborNum*LEN*LEN
	this->harborNum = harborCoords.size();
	harborsPaths = new Path * *[harborNum];
	for (int i = 0; i < harborNum; i++)
	{
		harborsPaths[i] = new Path * [LEN];
		for (int j = 0; j < LEN; j++)
		{
			harborsPaths[i][j] = new Path[LEN];
		}
	}

	for (int i = 0; i < harborNum; i++)
	{
		searchAllPath(my_map, harborCoords[i], harborsPaths[i]);

	}
}

vector<int> PathPlanner::getPathToHarbor(int harborId, Coord srcCoord)
{
	auto curHarborPath = harborsPaths[harborId];
	vector<int> moves;
	Coord p = srcCoord; // 出发地为指定坐标
	//Coord destCoord = harborCoord[harborId]; // 目的地为港口
	while (curHarborPath[p.x][p.y].distance != 0)
	{
		int curMove = curHarborPath[p.x][p.y].move;
		p = curHarborPath[p.x][p.y].lastCoord;
		moves.push_back(reverseMove[curMove]); // 反转方向
	}
	return moves;
}

vector<int> PathPlanner::getPathFromHarbor(int harborId, Coord destCoord)
{
	auto curHarborPath = harborsPaths[harborId];
	vector<int> moves;
	Coord p = destCoord; // 目的地为指定坐标
	//Coord srcCoord = harborCoord[harborId]; // 出发地为港口
	while (curHarborPath[p.x][p.y].distance != 0)
	{
		moves.push_back(curHarborPath[p.x][p.y].move);
		p = curHarborPath[p.x][p.y].lastCoord;
	}

	reverse(moves.begin(), moves.end()); // 反转路径
	return moves;
}

int PathPlanner::getDistanceToHarbor(int harborId, Coord srcCoord)
{
	return  harborsPaths[harborId][srcCoord.x][srcCoord.y].distance;
}

int PathPlanner::getEffectiveArea(int harborId)
{
	auto curHarborPath = harborsPaths[harborId];
	int count = 0;
	for (int i = 0; i < LEN; i++)
		for (int j = 0; j < LEN; j++)
		{
			if (curHarborPath[i][j].distance != -1)
				count++;
		}
	return count;
}