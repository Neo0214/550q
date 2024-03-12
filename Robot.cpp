#include "Robot.h"

Robot::Robot(int id, int _x, int _y) {
	this->id = id;
	this->x = _x;
	this->y = _y;
	hasGoods = NO_GOODS;
	status = ON;
}

void Robot::update(int hasGoods, int x, int y, int status) {
	if (this->hasGoods && !hasGoods)
		target = -1; // 已在港口放下物品
	if (!this->hasGoods && hasGoods)
		target = 1; // 已拿取物品
	this->hasGoods = hasGoods;
	this->x = x;
	this->y = y;
	this->status = status;
}

void Robot::assignTask(const vector<int>& moves, int target)
{
	this->moves = moves;
	this->curMoveCount = 0;
	this->target = target;
}

int Robot::moveOneStep(int collisionMap[LEN][LEN])
{
	if (curMoveCount < moves.size())
	{
		Coord nextPos = getNextPos();
		if (collisionMap[x][y] == 2 && (collisionMap[nextPos.x][nextPos.y] == 1 || collisionMap[nextPos.x][nextPos.y] == 2)) // 对撞情况
		{
			int i;
			for (i = 0; i < 4; i++) // 寻找一个方向避让
			{
				if (i == moves[curMoveCount])
					continue;
				Coord avoidPos = Coord(x, y) + i;
				if (avoidPos.x >= 0 && avoidPos.x < LEN && avoidPos.y >= 0 && avoidPos.y < LEN &&
					(collisionMap[avoidPos.x][avoidPos.y] == -1 || collisionMap[avoidPos.x][avoidPos.y] == 0))
				{
					collisionMap[avoidPos.x][avoidPos.y] = 2;
					printf("move %d %d\n", id, i);
					moves.insert(moves.begin() + curMoveCount, reverseMove[i]);
					break;
				}
			}
			if (i == 4) // 无法避让
			{

			}
		}
		else if (collisionMap[nextPos.x][nextPos.y] != 2) // 无碰撞
		{
			collisionMap[nextPos.x][nextPos.y] = 2;
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
		else // 目标点重合碰撞
		{
			collisionMap[x][y] = 2;
		}
	}
	else
	{
		//target = -1;
		if (atHarbor == -1)
			target = 1;
		else
			target = -1;


		if (collisionMap[x][y] == 2) 
		{
			int i;
			for (i = 0; i < 4; i++) // 寻找一个方向避让
			{
				Coord avoidPos = Coord(x, y) + i;
				if (avoidPos.x >= 0 && avoidPos.x < LEN && avoidPos.y >= 0 && avoidPos.y < LEN &&
					(collisionMap[avoidPos.x][avoidPos.y] == -1 || collisionMap[avoidPos.x][avoidPos.y] == 0))
				{
					collisionMap[avoidPos.x][avoidPos.y] = 2;
					printf("move %d %d\n", id, i);
					moves.insert(moves.begin() + curMoveCount, reverseMove[i]);
					break;
				}
			}
			if (i == 4) // 无法避让
			{

			}

		}
		else
		{
			collisionMap[x][y] = 2;
		}


	}
	return -1;
}

Coord Robot::getNextPos()
{
	if (curMoveCount < moves.size())
	{
		return Coord(x, y) + moves[curMoveCount];
	}

	return Coord();
}
