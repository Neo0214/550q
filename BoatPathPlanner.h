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
	vector<Edge> edges; // ���ڽӱ�
	int type;
	Node(int _type) { type = _type; }
};

class BoatPathPlanner {
private:
	vector<Node> berth; // ��������λ���ۿ�λ������λ
public:
	BoatPathPlanner();
	void initBoatPathPlanner(const char my_map[LEN][LEN]);
};