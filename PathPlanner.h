#pragma once
#include "defines.h"
#include "Map.h"


struct Path
{
	Coord lastCoord;
	int move;
	int distance;
	Path():move(-1),distance(-1) {};
};


class PathPlanner 
{
// ˽������
private:
	Path*** harborsPaths;
	Coord harborCoord[HARBOR_NUM];

	int reverseMove[4] = { 1,0,3,2 };

// ˽�к���
private:
	// ������startCoord��ʼ�ĵ����е��·��
	void searchAllPath(const int my_map[LEN][LEN],Coord startCoord, Path** path);


public:
	PathPlanner();
	// �Ե�ͼ��ʼ��·���滮����ȡ·��
	void initHarborPath(const int my_map[LEN][LEN],Coord coord[HARBOR_NUM]);

	vector<int> getPathToHarbor(int harborId, Coord srcCoord);

	vector<int> getPathFromHarbor(int harborId, Coord destCoord);

	int getDistanceToHarbor(int harborId, Coord srcCoord);

};