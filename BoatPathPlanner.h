#pragma once
#include "defines.h"
#include "Harbor.h"
#include "Delivery.h"

struct Node {
	char direct;
	short distance;
	Node() :direct(-1), distance(-1) {}
	Node(char _direct, int _distance) :direct(_direct), distance(_distance) {}
	void set(char _direct, int _distance) {
		this->direct = _direct;
		this->distance = _distance;
	}
};

class BoatPathPlanner {

private:
	Node*** map; // 0-右 1-左 2-上 3-下
	int harborNum;
	int sellPlaceNum;


public:
	BoatPathPlanner(int harborNum, int sellPlaceNum);
	BoatPathPlanner() {}
	~BoatPathPlanner();
	void init(char map[LEN][LEN], vector<Harbor>& harbors, vector<Delivery>& deliveries);
	void BFSearch(Node** pathMap, char map[LEN][LEN], Coord begin);
	int wholeBoatAvailable(char map[LEN][LEN], Coord& checkPos, int direct);
	void clean();
	int findBestSellPlace(Coord cur); // 返回最近的售卖点
	int nextMove(Coord curPos, int curDirect, int mapId, char originMap[LEN][LEN]);
	int getDistance(Coord curPos, int mapId);
	bool canForward(int curDirect, Coord curPos, char originMap[LEN][LEN]);
	int hasToChooseTurn(int curDirect, Coord curPos, char originMap[LEN][LEN]);
};