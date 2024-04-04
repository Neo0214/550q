#pragma once
#include "defines.h"
#include "Map.h"


struct Path
{
	Coord lastCoord; // ��һ�������
	int move; // ����һ�㵽�õ���ƶ�����
	int distance; // �Ӹۿڵ��õ�ľ���
	Path():move(-1),distance(-1) {}; // �����ʼ��Ϊ-1
};


class PathPlanner 
{
// ˽������
private:
	Path*** harborsPaths; // �����иۿڵ����е��·��
	//Coord harborCoord[harborNum]; // ��¼�ۿ�����

	

// ˽�к���
private:
	// ������startCoord��ʼ�ĵ����е��·��
	void searchAllPath(const char my_map[LEN][LEN],Coord startCoord, Path** path);


public:
	PathPlanner();

	// �Ե�ͼ��ʼ��·���滮����ȡ·��
	void initHarborPath(const char my_map[LEN][LEN],Coord coord[5]);

	// ��ȡ��srcCoord��harborId��·��
	vector<int> getPathToHarbor(int harborId, Coord srcCoord);

	// ��ȡ��harborId��destCoord��·��
	vector<int> getPathFromHarbor(int harborId, Coord destCoord);

	// ��ȡ��srcCoord��harborId��·������
	int getDistanceToHarbor(int harborId, Coord srcCoord);

};