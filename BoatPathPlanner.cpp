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
		Coord leftTop = harbors[i].getPos();
		if (my_map[leftTop.x][leftTop.y + 3] == HARBOR_SPACE) {
			// ˵���Ǻ��ŵ�
			if (my_map[leftTop.x - 1][leftTop.y] == LOAD_SPACE) {
				// ˵�����ϸ�����
				harbors[i].setBerthCoord(Coord(leftTop.x - 2, leftTop.y+1));
			}
			else {
				harbors[i].setBerthCoord(Coord(leftTop.x + 3, leftTop.y + 1));
			}
		}
		else {
			// ˵�������ŵ�
			if (my_map[leftTop.x][leftTop.y - 1] == LOAD_SPACE) {
				// ˵�����󸲸���
				harbors[i].setBerthCoord(Coord(leftTop.x + 1, leftTop.y - 2));
			}
			else {
				harbors[i].setBerthCoord(Coord(leftTop.x + 1, leftTop.y + 3));
			}
		}
	}

}

void BoatPathPlanner::initBoatPathPlanner(const char my_map[LEN][LEN], vector<Harbor>& harbors, const vector<Coord>& buyPlace, const vector<Coord>& deliveryPlace) {
	// ��ʼ��Node
	
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
	// ���ɸۿ�ͣ��Ŀ������
	generateBerthCoord(harbors, my_map);
	// Ѱ·ͼ����������
	// ����-->�ۿ�
	// �ۿ�-->�ۿ�
	// �ۿ�-->����
	// ����
	// ���ȴӹ��򵽸ۿڣ�����
	for (int i = 0; i < buyPlace.size(); i++) {
		BFSSearch(my_map, buyPlace[i]);
		// ��ʱ������ȫͼ�ķ�����Դͼ
		for (int j = harborStartIndex; j < deliveryStartIndex; j++) {
			vector<int> action = getActions(buyPlace[i], harbors[j].getBerthCoord());
			berth[i].edges.push_back(Edge(action, action.size()));
		}


		refreshPath();
	}
	// ���Ѹۿڵ��ۿ�
	for (int i = buyPlace.size(); i < buyPlace.size() + harbors.size(); i++) {

	}
	// ����Ѹۿڵ�����
	for (int i = buyPlace.size() + harbors.size(); i < berth.size(); i++) {

	}
}

bool BoatPathPlanner::isAvailable(char type) {
	return type == SEA_SPACE || type == MAIN_SEA || type == BUY_SHIP_SPACE ||
		type == HARBOR_SPACE || type == LOAD_SPACE || type == OVERPASS ||
		type == OVERPASS_MAIN || type == DELIVERY;
}

void BoatPathPlanner::BFSSearch(const char my_map[LEN][LEN], Coord start) {
	queue<Coord> q;
	q.push(start);
	this->path[start.x][start.y] = 0;
	while (!q.empty()) {
		Coord cur = q.front();
		q.pop();
		for (int i = 0; i < 4; i++) {
			Coord next = cur + i;
			if (isAvailable(my_map[next.x][next.y]) && path[next.x][next.y] == -1) {
				q.push(next);
				path[next.x][next.y] = i;
			}
		}
	}
	//FILE* fp = fopen("path.txt", "w");
	//for (int i = 0; i < LEN; i++) {
	//	for (int j = 0; j < LEN; j++) {
	//		if (path[i][j] == -1)
	//			fprintf(fp, "%c", my_map[i][j]);
	//		else if (path[i][j] == 0)
	//			fprintf(fp, "%s", "��");
	//		else if (path[i][j]==1)
	//			fprintf(fp,"%s","��");
	//		else if (path[i][j]==2)
	//			fprintf(fp,"%s","��");
	//		else if (path[i][j]==3)
	//			fprintf(fp,"%s","��");
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
		memset(&path[i], -1, sizeof(path[i]));
	}

}

vector<int> BoatPathPlanner::getActions(Coord start, Coord end) {
	vector<int> actions;
	Coord cur = end;
	while (cur != start) {

	}
	return actions;
}