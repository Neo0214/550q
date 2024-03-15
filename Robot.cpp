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

pair<int,int> Robot::moveOneStep(int collisionMap[LEN][LEN])
{
	//cerr << id << "find" << endl;
	// 返回值第一项代表移动方向，第二项代表冲突的机器人Id
	int expectedMove;
	Coord expectedPos;
	if (curMoveCount < moves.size())
	{
		expectedMove = moves[curMoveCount];
		expectedPos = Coord(x, y) + expectedMove;
	}
	else
	{
		expectedMove = -1;
		expectedPos = Coord(x, y);
	}
	//cerr << '(' << expectedPos.x << ',' << expectedPos.y << ')';
	// 冲突检测

	// 高16位为0，代表这个位置不是之前某个机器人的下一个位置，可以锁定该位置


	if (
		(collisionMap[expectedPos.x][expectedPos.y] >> 16) != 0
		|| (
			expectedMove != -1
			&&
			(collisionMap[expectedPos.x][expectedPos.y] & 0x0000ffff) != 0
			&&
			(collisionMap[x][y] >> 16) == (collisionMap[expectedPos.x][expectedPos.y] & 0xffff)
			)
		)
	{
		// 先判断能否停止
		if (collisionMap[x][y] >> 16 == 0)
		{ // 可以停止
			collisionMap[x][y] |= (id + 1) << 16; // 锁定位置
			return make_pair(-1, -1);
		}
		else
		{ // 不能停止，寻找一个方向避让
			int i;
			for (i = 0; i < 4; i++) // 遍历周围位置
			{
				if (i == expectedMove)
					continue;
				Coord avoidPos = Coord(x, y) + i;
				if (avoidPos.x >= 0 && avoidPos.x < LEN && avoidPos.y >= 0 && avoidPos.y < LEN 
					&& (collisionMap[avoidPos.x][avoidPos.y] >> 16) == 0
					&& !(
						(collisionMap[avoidPos.x][avoidPos.y] & 0xffff) != 0
						&&
						(collisionMap[x][y] >> 16) == (collisionMap[avoidPos.x][avoidPos.y] & 0xffff)
						)	
					)
				{
					collisionMap[avoidPos.x][avoidPos.y] |= (id + 1) << 16; // 锁定位置
					return make_pair(i, -1); // 这里直接return了，如果不直接return需要break
				}
			}
			if (i == 4) // 无法避让
			{ // 强制前进，发出冲突机器人编号
				int collisionId; // 这里存储的是+1后的id
				if ((collisionMap[expectedPos.x][expectedPos.y] >> 16) != 0)
				{
					collisionId = collisionMap[expectedPos.x][expectedPos.y] >> 16;
				}
				if (expectedMove != -1
					&&
					(collisionMap[expectedPos.x][expectedPos.y] & 0xffff) != 0
					&&
					(collisionMap[x][y] >> 16) == (collisionMap[expectedPos.x][expectedPos.y] & 0xffff))
				{
					collisionId = min(collisionId, collisionMap[x][y] >> 16);
				}
				collisionMap[expectedPos.x][expectedPos.y] |= (id + 1) << 16; // 锁定位置
				return make_pair(expectedMove, collisionId-1);
			}
		}
	}
	else
	{
		// 锁定该位置，把高16位设置为这个机器人id+1
		collisionMap[expectedPos.x][expectedPos.y] |= (id + 1) << 16; // 锁定位置
		return make_pair(expectedMove, -1);
	}

	return make_pair(-1, -1); // 不会到达此句
	
}

int Robot::command(int realMove)
{
	//cerr << id << "move" << realMove<<endl;
	if (realMove >= 0)
		printf("move %d %d\n", id, realMove);
	if (curMoveCount < moves.size())
	{
		if (moves[curMoveCount] == realMove) // 正常移动
		{
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
		else // 非正常移动或暂停
		{
			if (realMove == -1)
				; // do nothing
			else
				moves.insert(moves.begin() + curMoveCount, reverseMove[realMove]);
		}
	}
	else
	{
		if (realMove == -1) // 正常暂停
		{
			if (atHarbor == -1)
				target = 1;
			else
				target = -1;
		}
		else // 被强制移动
		{
			moves.insert(moves.begin() + curMoveCount, reverseMove[realMove]);
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
