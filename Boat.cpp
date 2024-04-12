#include "Boat.h"
#include "BoatPathPlanner.h"
Boat::Boat(int _id, int _capacity)
{
	id = _id;
	capacity = _capacity;
	status = MOVING;
	pos = Coord(-1, -1);
	curCapacity = 0;
	preLoadNum = 0;
	curValue = 0;
	orders = vector<Order>();
	turns = 0;
	direction = 0;
	action = vector<int>();
	curAct = 0;
	prePos = Coord(-1, -1);
	force = true;
	target = vector<int>();
	preTarget = -1;

}
Boat::Boat() {

}

void Boat::update(int status, int curCapacity, int x, int y, int direction) {

	this->status = status;
	this->curCapacity = curCapacity;
	this->prePos = this->pos;
	this->pos = Coord(x, y);
	this->direction = direction;
}



int Boat::getStatus() {
	return status;
}

int Boat::getPos() {
	return 0;
}

int Boat::getCurCapacity() {
	return curCapacity;
}

void Boat::addGoods(int num, int value) {
	curCapacity += num;
	preLoadNum = num;
	curValue += value;
	orders[0].minus(num);
}

int Boat::getPre() {
	return preLoadNum;
}

int Boat::getCurValue() {
	return curValue;
}

void Boat::clearOrders() {
	this->orders.clear();
}

void Boat::clearOneOrder() {
	this->orders.erase(this->orders.begin(), this->orders.begin() + 1);
}

void Boat::addOneOrder() {
	this->orders[0].add();
}

void Boat::newOrder(int goodsLeft, int harborId) {
	Order order(harborId, goodsLeft);
	this->orders.push_back(order);
}

int Boat::originPos() {
	return this->orders[0].id;
}

int Boat::getOrderCapacity() {
	return this->orders[0].productNumber;
}

void Boat::act(int act)
{
	if (act == FORWARD) {
		forward();
	}
	else if (act == DRIVEIN) {
		driveIn();
	}
	else if (act == LEAVE) {
		leave();
	}
	else if (act != -1) {
		rot(act - 1);
	}
}
void Boat::leave()
{
	printf("dept %d\n", id);
}
void Boat::driveIn()
{
	printf("berth %d\n", id);
}
void Boat::rot(int rotDirect)
{
	printf("rot %d %d\n", id, rotDirect);
}
void Boat::forward()
{
	printf("ship %d\n", id);
}

bool Boat::isFree()
{
	return this->target == -1;
}

void Boat::nextAct(int harborNum, BoatPathPlanner* boatPathPlanner)
{
	cerr << "status" << status << endl;
	if (curAct == action.size() && status == MOVING) {
		cerr << "dirvein" << endl;
		cerr << "pos: " << pos << endl;
		cerr << "dir:" << direction << endl;
		boatPathPlanner->clearClps(BoatState(pos, direction), id);
		cerr << "drivein-ok" << endl;
		act(DRIVEIN);
		if (target >= harborNum) {
			preTarget = target;
			target = -1;
		}
		curAct++;
		return;
	}
	else if (curAct < action.size()) {

		if (status == RECOVER)
			return;
		else if (status == MOVING) {
			if (boatPathPlanner->isOKNextMove(pos, direction, id, action[curAct])) {
				act(action[curAct++]);
			}
		}
	}
}