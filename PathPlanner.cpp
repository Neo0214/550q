#include "PathPlanner.h"

PathPlanner::PathPlanner()
{
	// 为harborsPaths分配内存为HARBOR_NUM*LEN*LEN
	haborsPaths = new Path**[HARBOR_NUM];
	for (int i = 0; i < HARBOR_NUM; i++)
	{
		haborsPaths[i] = new Path*[LEN];
		for (int j = 0; j < LEN; j++)
		{
			haborsPaths[i][j] = new Path[LEN];
		}
	}
}


void PathPlanner::searchAllPath(const int my_map[LEN][LEN], Coord startCoord, Path** path) // path是得到的路径矩阵
{
	queue<Coord> q;
	bool visited[LEN][LEN] = { { 0 } };
	q.push(startCoord);
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
			}
		}
	}
}



void PathPlanner::initHarborPath(const int my_map[LEN][LEN],Coord coord[HARBOR_NUM])
{
	for (int i = 0; i < HARBOR_NUM; i++)
	{
		haborCoord[i]= coord[i]; // 记录港口坐标
		searchAllPath(my_map,coord[i],haborsPaths[i]);
	}
}

vector<int> PathPlanner::getPathToHarbor(int haborId, Coord srcCoord)
{
	auto curHaborPath = haborsPaths[haborId];
	vector<int> moves;
	Coord p = srcCoord;
	Coord destCoord = haborCoord[haborId];
	while (p.x != destCoord.x || p.y != destCoord.y)
	{
		int curMove = curHaborPath[p.x][p.y].move;
		cerr << curMove;
		p = curHaborPath[p.x][p.y].lastCoord;
		moves.push_back(reverseMove[curMove]);
	}
	return moves;
}

vector<int> PathPlanner::getPathFromHarbor(int haborId, Coord destCoord)
{
	auto curHaborPath = haborsPaths[haborId];
	vector<int> moves;
	Coord p = destCoord;
	while (p.x != destCoord.x || p.y != destCoord.y)
	{
		p = curHaborPath[p.x][p.y].lastCoord;
		moves.push_back(curHaborPath[p.x][p.y].move);
	}

	reverse(moves.begin(),moves.end());
	return moves;
}