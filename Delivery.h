#pragma once
#include "defines.h"
#include "Coord.h"

class Delivery {
private:
	int id;
	Coord pos;

public:
	Delivery();
	Delivery(Coord _pos, int _id);
	Coord getPos();
};