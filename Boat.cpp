#include "Boat.h"

Boat::Boat(int _id, int _capacity)
{
	id = _id;
	capacity = _capacity;
	status = NORMAL;
	pos = -1;
	curCapacity = 0;
}
Boat::Boat() {

}

void Boat::update(int status, int pos) {
	this->status = status;
	this->pos = pos;
}

void Boat::gotoHarbor(int harborId) {
	printf("ship %d %d\n", id, harborId);
	cerr<<"ship "<<id<<" goto "<<harborId<<endl;
	this->status= MOVING;
}

void Boat::comeBack() {
	printf("go %d\n", id);
	this->curCapacity = 0;
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

void Boat::addGoods(int num) {
	curCapacity += num;
}