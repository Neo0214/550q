#pragma once
#include "defines.h"
#include "Map.h"
#include "Coord.h"

struct Path
{
	Coord lastCoord; // 上一点的坐标
	int move; // 从上一点到该点的移动方向
	int distance; // 从港口到该点的距离
	Path() :move(-1), distance(-1) {}; // 距离初始化为-1
};


class PathPlanner
{
	// 私有数据
private:
	Path*** harborsPaths; // 从所有港口到所有点的路径
	int harborNum; // 港口数量

	// 私有函数
private:
	// 搜索从startCoord开始的到所有点的路径
	void searchAllPath(const char my_map[LEN][LEN], Coord startCoord, Path** path);


public:
	PathPlanner() {};

	// 以地图初始化路径规划器获取路径
	void initHarborPath(const char my_map[LEN][LEN], vector<Coord> harborCoords);

	// 获取从srcCoord到harborId的路径
	vector<int> getPathToHarbor(int harborId, Coord srcCoord);

	// 获取从harborId到destCoord的路径
	vector<int> getPathFromHarbor(int harborId, Coord destCoord);

	// 获取从srcCoord到harborId的路径长度
	int getDistanceToHarbor(int harborId, Coord srcCoord);

	int getEffectiveArea(int harborId);

};