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
		if (my_map[leftTop.x][leftTop.y + 2] == HARBOR_SPACE) {
			// ˵���Ǻ��ŵ�
			if (my_map[leftTop.x - 1][leftTop.y] == LOAD_SPACE) {
				// ˵�����ϸ�����
				harbors[i].setBerthCoord(Coord(leftTop.x - 2, leftTop.y + 1));
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

void BoatPathPlanner::initBoatPathPlanner(const char my_map[LEN][LEN], vector<Harbor>& harbors, const vector<Coord>& buyPlace, const int harborNum) {
	// ��ʼ��Node
	
	for (int i = 0; i < buyPlace.size(); i++) {
		berth.push_back(Node(BUY_TYPE));
	}
	for (int i = 0; i < harbors.size(); i++) {
		berth.push_back(Node(HARBOR_TYPE));
	}
	int harborStartIndex= buyPlace.size();
	int deliveryStartIndex= buyPlace.size() + harborNum;
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
			vector<int> action = getActions(buyPlace[i], harbors[j-buyPlace.size()].getBerthCoord());
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
	cerr << path[101][26]<< endl;

	this->path[start.x][start.y] = 0;
	cerr << path[101][26] << endl;
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
		memset(path[i], -1, sizeof(int) * LEN);
	}
}

vector<int> BoatPathPlanner::getActions(Coord start, Coord end) {
	vector<int> reversedAction;
	Coord cur = end;
	// ��Դ����ȡ�������
	while (cur != start) {
		reversedAction.push_back(path[cur.x][cur.y]);
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
	// ���ƣ���ȡ�������
	vector<int> forwardActions;
	for (int i = reversedAction.size() - 1; i >= 0; i--) {
		forwardActions.push_back(reversedAction[i]);
	}
	// ������ת�����ɵ������ӳ٣��������ղ�������
	vector<int> actions;

	return actions;
}