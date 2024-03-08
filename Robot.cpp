#include "Robot.h"

Robot::Robot(int _x, int _y) {
	x=_x;
	y=_y;
	hasGoods=NO_GOODS;
	status=ON;
}
Robot::Robot() {

}

void Robot::update(int hasGoods, int x, int y, int status) {
	if (this->hasGoods && !hasGoods) 
		target = -1; // 已在港口放下物品
	this->hasGoods=hasGoods;
	this->x=x;
	this->y=y;
	this->status=status;
}

void Robot::assignTask(const vector<int>& moves, int target)
{
	this->moves = moves;
	this->curMoveCount = 0;
	this->target = target;
}
