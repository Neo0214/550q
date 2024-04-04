#pragma once
#include "defines.h"

struct Edge
{
	vector<int> actions;
	int length;
	Edge(vector<int>& _actions, int _length) { actions = _actions; length = _length; }
};

struct Node
{
	vector<Edge> edges; // 用邻接表
	int type;
	Node(int _type) { type = _type; }
};

class BoatPathPlanner {
private:
	vector<Node> berth; // 包括购买位，港口位，交货位
public:
	BoatPathPlanner();
	void initBoatPathPlanner(const char my_map[LEN][LEN]);
};