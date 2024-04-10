#pragma once
#include "defines.h"
#include "Harbor.h"
#include "Delivery.h"

struct Node {
	short direct;
	short distance;
	Node() :direct(-1), distance(-1) {}
	Node(short _direct, int _distance) :direct(_direct), distance(_distance) {}
	void set(short _direct, int _distance) {
		this->direct = _direct;
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

	void clean();

	void searchAllPath(const char my_map[LEN][LEN], vector<Coord> startCoord, Node*** path);

};