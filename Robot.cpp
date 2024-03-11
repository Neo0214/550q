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
	if (!this->hasGoods && hasGoods)
		target = 1; // 已拿取物品
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

int Robot::moveOneStep()
{
	if (curMoveCount < moves.size())
	{
		printf("move %d %d\n", id, moves[curMoveCount]);
		curMoveCount++;
		if (curMoveCount == moves.size()) // 到达目标位置
		{
			if (!hasGoods) {
				printf("get %d\n", id);
				return 0;
			}
			else {
				printf("pull %d\n", id);
				return 1;
			}
		}
	}
	else
	{
		target = -1;
#ifdef DEBUG
		cerr << id << " fails to move" << endl;
#endif
	}
	return -1;
}
