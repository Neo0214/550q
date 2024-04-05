#pragma once



#include "defines.h"
#include "Coord.h"
#include "Harbor.h"
#include "Delivery.h"

#define BUY_TYPE 0
#define DELIVERY_TYPE 1
#define HARBOR_TYPE 2


struct Edge
{
	vector<int> actions;
	int length;
	int target;
	Edge(vector<int>& _actions, int _length, int _target) { actions = _actions; length = _length; target = _target; }
};

struct Node
{
	vector<Edge> edges; // ���ڽӱ�
	int type;
	Node(int _type) { type = _type; edges = vector<Edge>(); }
};

class BoatPathPlanner {
private:
	vector<Node> berth; // ��������λ���ۿ�λ������λ
	int** path; // ��ʱ�洢������Դͼ
public:
	BoatPathPlanner();
	void initBoatPathPlanner(const char my_map[LEN][LEN], vector<Harbor>& harbors, const vector<Coord>& buyPlace, const vector<Delivery>& deliveryPlace);
	void generateBerthCoord(vector<Harbor>& harbors, const char my_map[LEN][LEN]);
	void BFSSearch(const char my_map[LEN][LEN], Coord start);
	void refreshPath();
	bool isAvailable(const char type);
	vector<int> getActions(Coord start, Coord end);
	int getChangingStatus(int preAct, int act);
};