#include "Delivery.h"


Delivery::Delivery(Coord _pos, int _id) {
	this->pos=Coord(_pos.x,_pos.y);
	this->id = _id;
}