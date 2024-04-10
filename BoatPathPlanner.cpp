#include "BoatPathPlanner.h"

BoatPathPlanner::BoatPathPlanner(int harborNum, int sellPlaceNum) {
	this->harborNum = harborNum;
	this->sellPlaceNum = sellPlaceNum;

}
BoatPathPlanner::~BoatPathPlanner() {
	// 释放内存
	//for (int i = 0; i < harborNum + sellPlaceNum; i++) {
	//	for (int j = 0; j < LEN; j++) {
	//		for (int k = 0; k < LEN; k++) {
	//			delete[] this->map[i][j][k];
	//		}
	//		delete[] this->map[i][j];
	//	}
	//	delete[] this->map[i];
	//}
	//delete[] this->map;
}
void BoatPathPlanner::clean() {
	for (int i = 0; i < harborNum + sellPlaceNum; i++) {
		for (int j = 0; j < LEN; j++) {
			for (int k = 0; k < LEN; k++) {
				delete[] this->map[i][j][k];
			}
			delete[] this->map[i][j];
		}
		delete[] this->map[i];
	}
	delete[] this->map;

}

void BoatPathPlanner::init(char map[LEN][LEN], vector<Harbor>& harbors, vector<Delivery>& deliveries) {
	this->map = new Node * **[harborNum + sellPlaceNum];
	for (int i = 0; i < harborNum + sellPlaceNum; i++) {
		this->map[i] = new Node * *[LEN];
		for (int j = 0; j < LEN; j++) {
			this->map[i][j] = new Node * [LEN];
			for (int k = 0; k < LEN; k++) {
				this->map[i][j][k] = new Node[4];
			}
		}
	}
	for (int i = 0; i < harborNum; i++) {



	}
	for (int i = harborNum; i < harborNum + sellPlaceNum; i++) {
		// 对每个售卖点生成前往路径地图



	}
}
