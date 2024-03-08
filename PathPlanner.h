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
// ˽������
private:
	Path*** haborsPaths;
	Coord haborCoord[HARBOR_NUM];

	int reverseMove[4] = { 1,0,3,2 };

// ˽�к���
private:
	// ������startCoord��ʼ�ĵ����е��·��
	void searchAllPath(const int my_map[LEN][LEN],Coord startCoord, Path** path);


public:
	PathPlanner();
	// �Ե�ͼ��ʼ��·���滮����ȡ·��
	void initHarborPath(const int my_map[LEN][LEN],Coord coord[HARBOR_NUM]);

	vector<int> getPathToHarbor(int haborId, Coord srcCoord);

	vector<int> getPathFromHarbor(int haborId, Coord destCoord);



};