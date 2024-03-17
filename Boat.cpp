#include "Boat.h"

Boat::Boat(int _id, int _capacity)
{
	id = _id;
	capacity = _capacity;
	status = NORMAL;
	pos = -1;
	curCapacity = 0;
	preLoadNum = 0;
	curValue = 0;
}
Boat::Boat() {

}

void Boat::update(int status, int pos) {
	this->status = status;
	this->pos = pos;
}

void Boat::gotoHarbor(int harborId) {
	printf("ship %d %d\n", id, harborId);
	cerr << "boat " << id << " goto " << harborId << " with cap " << curCapacity << endl;
	this->pos= harborId;
	this->status= MOVING;
	
}

void Boat::comeBack() {
	printf("go %d\n", id);
	cerr << "boat " << id << " come back with cap " << this->curCapacity << endl;
	this->pos = -1;
	this->status = MOVING;
	this->curCapacity = 0;
	this->curValue = 0;
}

int Boat::getStatus() {
	return status;
}

int Boat::getPos() {
	return pos;
}

int Boat::getCurCapacity() {
	return curCapacity;
}

void Boat::addGoods(int num, int value) {
	curCapacity += num;
	preLoadNum = num;
	curValue += value;
}

int Boat::getPre() {
	return preLoadNum;
}

int Boat::getCurValue() {
	return curValue;
}