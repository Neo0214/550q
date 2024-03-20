#include "Harbor.h"

Harbor::Harbor(int _Id,int _x, int _y, int _time, int _velocity)
{
	Id = _Id;
	x = _x;
	y = _y;
	time = _time;
	velocity = _velocity;
	orders=vector<Order>();
	area = 0;
}
Harbor::Harbor() {

}

void Harbor::clearOneOrder() {
	this->orders.erase(this->orders.begin(), this->orders.begin() + 1);
	//this->endingCost+=0.5;
}

void Harbor::addOneOrder() {
	this->orders[0].add();
}

void Harbor::newOrder(int goodsLeft, int boatId) {
	Order order(boatId, goodsLeft);
	orders.push_back(order);
	//this->endingCost-=0.5;
}