#pragma once
#include "defines.h"
#include "Map.h"

constexpr int LEN = 200;


struct Path
{
	Coord lastCoord;
	int move;
	Path(Coord lastCoord, int move) :lastCoord(lastCoord), move(move) {};
	Path():move(-1) {};
};


class PathPlanner 
{
// 私有数据
private:
	Path*** haborsPaths;
	Coord haborCoord[HARBOR_NUM];

	int reverseMove[4] = { 1,0,3,2 };

// 私有函数
private:
	// 搜索从startCoord开始的到所有点的路径
	void searchAllPath(const int my_map[LEN][LEN],Coord startCoord, Path** path);


public:
	PathPlanner();
	// 以地图初始化路径规划器获取路径
	void initHarborPath(const int my_map[LEN][LEN],Coord coord[HARBOR_NUM]);

	vector<int> getPathToHarbor(int haborId, Coord srcCoord);

	vector<int> getPathFromHarbor(int haborId, Coord destCoord);



};