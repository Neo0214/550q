#include "PathPlanner.h"

PathPlanner::PathPlanner()
{
	// 为harborsPaths分配内存为HARBOR_NUM*LEN*LEN
	harborsPaths = new Path**[HARBOR_NUM];
	for (int i = 0; i < HARBOR_NUM; i++)
	{
		harborsPaths[i] = new Path*[LEN];
		for (int j = 0; j < LEN; j++)
		{
			harborsPaths[i][j] = new Path[LEN];
		}
	}
}


void PathPlanner::searchAllPath(const int my_map[LEN][LEN], Coord startCoord, Path** path) // path是得到的路径矩阵
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

		for (int i = 0; i < 4; i++) //TODO：随机打乱
		{
			Coord neighbor = neighbors[i];
			if (neighbor.x >= 0 && neighbor.x < LEN && neighbor.y >= 0 && neighbor.y < LEN
				&& my_map[neighbor.x][neighbor.y] >= -1 && !visited[neighbor.x][neighbor.y])
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



void PathPlanner::initHarborPath(const int my_map[LEN][LEN],Coord coord[HARBOR_NUM])
{
	for (int i = 0; i < HARBOR_NUM; i++)
	{
		harborCoord[i]= coord[i]; // 记录港口坐标
		searchAllPath(my_map,coord[i],harborsPaths[i]);
	}
}

vector<int> PathPlanner::getPathToHarbor(int harborId, Coord srcCoord)
{
	auto curHarborPath = harborsPaths[harborId];
	vector<int> moves;
	Coord p = srcCoord; // 出发地为指定坐标
	Coord destCoord = harborCoord[harborId]; // 目的地为港口
	while (p.x != destCoord.x || p.y != destCoord.y)
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
	Coord srcCoord = harborCoord[harborId]; // 出发地为港口
	while (p.x != srcCoord.x || p.y != srcCoord.y)
	{
		p = curHarborPath[p.x][p.y].lastCoord;
		moves.push_back(curHarborPath[p.x][p.y].move);
	}

	reverse(moves.begin(),moves.end()); // 反转路径
	return moves;
}

int PathPlanner::getDistanceToHarbor(int harborId, Coord srcCoord)
{
	return  harborsPaths[harborId][srcCoord.x][srcCoord.y].distance;
}
