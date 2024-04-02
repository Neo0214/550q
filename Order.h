#pragma once

#include "defines.h"

class Order {
	friend class Boat;
	friend class Harbor;
	friend class Scheduler;
private:
	int id; // 在港口中的订单，id是船号；在船中的订单，id是港口号
	int productNumber; // 订单中的产品数量

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