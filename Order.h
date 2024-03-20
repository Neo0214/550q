#pragma once

#include "defines.h"

class Order {
	friend class Boat;
	friend class Harbor;
	friend class Scheduler;
private:
	int id; // �ڸۿ��еĶ�����id�Ǵ��ţ��ڴ��еĶ�����id�Ǹۿں�
	int productNumber; // �����еĲ�Ʒ����

public:
	Order(int _id, int _productNumber) {
		id = _id;
		productNumber = _productNumber;
	}
	void add() {
		productNumber++;
	}
	void minus(int number) {
		productNumber -= number;
	}
};