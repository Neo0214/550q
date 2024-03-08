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
