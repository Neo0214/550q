#include "Robot.h"

Robot::Robot(int _x, int _y) {
	x=_x;
	y=_y;
	goods=NO_GOODS;
	status=ON;
}
Robot::Robot() {

}

void Robot::update(int hasGoods, int x, int y, int status) {
	goods=hasGoods;
	this->x=x;
	this->y=y;
	this->status=status;
}