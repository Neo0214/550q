#pragma once
#include "defines.h"
#include "Harbor.h"
#include "Delivery.h"
#include "BoatState.h"

struct Node {
	short move;
	short distance;
	Node() :move(-1), distance(-1) {}
	Node(short _direct, int _distance) :move(_direct), distance(_distance) {}
	void set(short _direct, int _distance) {
		this->move = _direct;
		this->distance = _distance;
	}
};

class BoatPathPlanner {

private:
	Node**** map; // 0-右 1-左 2-上 3-下
	int harborNum;
	int sellPlaceNum;


public:
	BoatPathPlanner(int harborNum, int sellPlaceNum);
	BoatPathPlanner() {}
	~BoatPathPlanner();
	void init(char map[LEN][LEN], vector<Harbor>& harbors, vector<Delivery>& deliveries);
	bool isLegalWholeBoat(BoatState neighbor, const char my_map[LEN][LEN]);
	void clean();
	void searchAllPath(const char my_map[LEN][LEN], vector<Coord> startCoord, Node*** path);
	vector<int> getPath(BoatState start, int targetId);
	void update(BoatState& cur, short move);
	int getDistance(BoatState& cur, int targetId);
};