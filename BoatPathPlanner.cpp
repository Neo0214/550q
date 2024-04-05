#include "BoatPathPlanner.h"


BoatPathPlanner::BoatPathPlanner() {
	berth=vector<Node>();
	path = new int*[LEN];
	for (int i = 0; i < LEN; i++) {
		path[i] = new int[LEN];
		memset(path[i], -1, sizeof(int)*LEN);
	}
}

void BoatPathPlanner::generateBerthCoord(vector<Harbor>& harbors, const char my_map[LEN][LEN]) {
	for (int i = 0; i < harbors.size(); i++) {
		Coord leftTop = harbors[i].boatCoord;
		cerr << leftTop << endl;
		if (my_map[leftTop.x][leftTop.y + 2] == HARBOR_SPACE) {
			// 说明是横着的
			if (my_map[leftTop.x - 1][leftTop.y] == LOAD_SPACE) {
				// 说明是上覆盖型
				harbors[i].berthCoord=Coord(leftTop.x - 2, leftTop.y + 1);
			}
			else {
				harbors[i].berthCoord=Coord(leftTop.x + 3, leftTop.y + 1);
			}
		}
		else {
			// 说明是竖着的
			if (my_map[leftTop.x][leftTop.y - 1] == LOAD_SPACE) {
				// 说明是左覆盖型
				harbors[i].berthCoord=Coord(leftTop.x + 1, leftTop.y - 2);
			}
			else {
				harbors[i].berthCoord=Coord(leftTop.x + 1, leftTop.y + 3);
			}
		}
	}
}

void BoatPathPlanner::initBoatPathPlanner(const char my_map[LEN][LEN], vector<Harbor>& harbors, const vector<Coord>& buyPlace, const vector<Delivery>& deliveryPlace) {
	// 初始化Node
	cerr << "into initBoatPathPlanner" << endl;
	for (int i = 0; i < buyPlace.size(); i++) {
		berth.push_back(Node(BUY_TYPE));
	}
	for (int i = 0; i < harbors.size(); i++) {
		berth.push_back(Node(HARBOR_TYPE));
	}
	for (int i = 0; i < deliveryPlace.size(); i++) {
		berth.push_back(Node(DELIVERY_TYPE));
	}
	int harborStartIndex= buyPlace.size();
	int deliveryStartIndex= buyPlace.size() + harbors.size();
	// 生成港口停靠目标坐标
	//generateBerthCoord(harbors, my_map);
	cerr << "out generateBerthCoord" << endl;
	// 寻路图有三个部分
	// 购买-->港口
	// 港口-->港口
	// 港口-->交货
	// 出表
	// 首先从购买到港口，广搜
	for (int i = 0; i < buyPlace.size(); i++) {
		BFSSearch(my_map, buyPlace[i]);
		// 此时已生成全图的方向溯源图
		for (int j = harborStartIndex; j < deliveryStartIndex; j++) {

			cerr << "generate " << i << " " << j-harborStartIndex << endl;
			vector<int> action = getActions(buyPlace[i], harbors[j - harborStartIndex].boatCoord);
			//for (int k = 0; k < action.size(); k++) {
			//	cerr << action[k] << " ";
			//}
			//cerr << endl;
			berth[i].edges.push_back(Edge(action, action.size(),j-harborStartIndex));
		}
		refreshPath();
	}
	cerr << "for 1" << endl;
	// 再搜港口到港口
	for (int i = buyPlace.size(); i < buyPlace.size() + harbors.size(); i++) {
		BFSSearch(my_map, harbors[i - buyPlace.size()].boatCoord);
		cerr << "ok";
		for (int j = harborStartIndex; j < deliveryStartIndex; j++) {
			if (i == j) {
				continue;
			}
			cerr << "generate " << i << " " << j - harborStartIndex << endl;
			int realIndex = j - harborStartIndex;
			vector<int> action=getActions(harbors[i - buyPlace.size()].boatCoord, harbors[realIndex].boatCoord);
			berth[i].edges.push_back(Edge(action,action.size(), realIndex));
		}
		refreshPath();
	}
	cerr << "for 2" << endl;
	// 最后搜港口到交货
	for (int i = buyPlace.size(); i < buyPlace.size() + harbors.size(); i++) {
		BFSSearch(my_map, harbors[i - buyPlace.size()].boatCoord);
		for (int j = deliveryStartIndex; j < berth.size(); j++) {
			int realIndex = j - deliveryStartIndex;
			cerr << "generate " << i << " " <<realIndex << endl;
			vector<int> action = getActions(harbors[i - buyPlace.size()].boatCoord, harbors[realIndex].boatCoord);
			berth[i].edges.push_back(Edge(action, action.size(), -realIndex));
		}
		refreshPath();
	}
	cerr << "for 3" << endl;
}

bool BoatPathPlanner::isAvailable(char type) {
	return type == SEA_SPACE || type == MAIN_SEA || type == BUY_SHIP_SPACE ||
		type == HARBOR_SPACE || type == LOAD_SPACE || type == OVERPASS ||
		type == OVERPASS_MAIN || type == DELIVERY;
}

void BoatPathPlanner::BFSSearch(const char my_map[LEN][LEN], Coord start) {
	cerr << "start BFS" << endl;
	cerr << start << endl;
	queue<Coord> q;
	q.push(start);
	this->path[start.x][start.y] = 0;
	int times = 0;
	while (!q.empty()) {
		Coord cur = q.front();
		q.pop();
		for (int i = 0; i < 4; i++) {
			Coord next = cur + i;
			if (next.x < 0 || next.x >= LEN || next.y < 0 || next.y >= LEN) {
				continue;
			}
			//if (start.x != 49)
			//	cerr << (isAvailable(my_map[next.x][next.y]) && path[next.x][next.y] == -1) << endl;
			if (isAvailable(my_map[next.x][next.y]) && path[next.x][next.y] == -1) {
				//if (start.x != 49)
					//cerr << times++ << endl;
				q.push(next);
				path[next.x][next.y] = i;
			}
		}
		
	}
	
	cerr << "BFS done" << endl;
	//FILE* fp = fopen("path.txt", "w");
	//for (int i = 0; i < LEN; i++) {
	//	for (int j = 0; j < LEN; j++) {
	//		if (path[i][j] == -1)
	//			fprintf(fp, "%c", my_map[i][j]);
	//		else if (path[i][j] == 0)
	//			fprintf(fp, "%s", "0");
	//		else if (path[i][j]==1)
	//			fprintf(fp,"%s","1");
	//		else if (path[i][j]==2)
	//			fprintf(fp,"%s","2");
	//		else if (path[i][j]==3)
	//			fprintf(fp,"%s","3");
	//
	//	}
	//	fprintf(fp, "\n");
	//}
	//fclose(fp);
}

void BoatPathPlanner::refreshPath() {
	for (int i = 0; i < LEN; i++) {
		delete[] path[i];
	}
	delete[] path;

	path=new int*[LEN];
	for (int i = 0; i < LEN; i++) {
		path[i] = new int[LEN];
		memset(path[i], -1, sizeof(int) * LEN);
	}
}

vector<int> BoatPathPlanner::getActions(Coord start, Coord end) {
	vector<int> reversedAction;
	Coord cur = end;
	// 溯源。获取反向操作
	while (cur != start) {
		reversedAction.push_back(path[cur.x][cur.y]);
		//cerr << cur << endl;
		//cerr << path[cur.x][cur.y] << endl;
		switch (path[cur.x][cur.y])
		{
		case 0:
			cur.y--;
			break;
		case 1:
			cur.y++;
			break;
		case 2:
			cur.x++;
			break;
		case 3:
			cur.x--;
			break;
		default:
			break;
		}
	}
	reversedAction.push_back(path[cur.x][cur.y]);
	// 倒推，获取正向操作
	vector<int> forwardActions;
	for (int i = reversedAction.size() - 1; i >= 0; i--) {
		forwardActions.push_back(reversedAction[i]);
	}
	// 考虑旋转和主干道操作延迟，生成最终操作序列
	vector<int> actions;
	int preAct = 0; // 初始状态向右
	for (int i = 0; i < forwardActions.size(); i++) {
		int act = forwardActions[i];
		switch (getChangingStatus(preAct, act)) {
		case 0:
			actions.push_back(FORWARD);
			break;
		// 有转动时，先插入旋转操作，再加入前进 
		case 1:
			actions.push_back(LEFTTURN);
			actions.push_back(FORWARD);
			break;
		case 2:
			actions.push_back(LEFTTURN);
			actions.push_back(LEFTTURN);
			actions.push_back(LEFTTURN);
			actions.push_back(FORWARD);
			break;
		}
		preAct = act;
	}
	return actions;
}

int BoatPathPlanner::getChangingStatus(int preAct, int act) {
	if (preAct == act)
		return 0;
	if (preAct <= 1) {
		switch ((preAct+act)%2)
		{
		case 0:
			return 1;
			break;
		case 1:
			return 2;
			break;
		default:
			break;
		}
	}
	else {
		switch ((preAct + act) % 2) {
		case 1:
			return 1;
			break;
		case 0:
			return 2;
			break;
		default:
			break;
		}
	}
	return 0;
}