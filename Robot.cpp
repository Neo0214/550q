#include "Robot.h"

Robot::Robot(int id, int _x, int _y) {
	this->id = id;
	this->x=_x;
	this->y=_y;
	hasGoods=NO_GOODS;
	status=ON;
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

void Robot::moveOneStep()
{
	if (curMoveCount < moves.size())
	{
		printf("move %d %d\n", id, moves[curMoveCount]);
		curMoveCount++;
	}
	else
	{
		cerr << id << " fails to move" <<endl;
		// printf("move %d %d\n", id, 1);
	}
}
