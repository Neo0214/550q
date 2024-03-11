#include "Boat.h"

Boat::Boat(int _id, int _capacity)
{
	id = _id;
	capacity = _capacity;
	status = NORMAL;
	pos = -1;
}
Boat::Boat() {

}

void Boat::update(int status, int pos) {
	this->status = status;
	this->pos = pos;
}

void Boat::gotoHarbor(int harborId) {
	printf("ship %d %d\n", id, harborId);
	this->status= MOVING;
}

void Boat::comeBack() {
	printf("go %d\n", id);
}

int Boat::getStatus() {
	return status;
}

int Boat::getPos() {
	return pos;
}