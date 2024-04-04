#include "Robot.h"

Robot::Robot(int id, int _x, int _y) {
	this->id = id;
	this->x = _x;
	this->y = _y;
	hasGoods = NO_GOODS;
	status = ON;
}

void Robot::update(int hasGoods, int x, int y) {
	if (this->hasGoods && !hasGoods)
		target = -1; // 已在港口放下物品
	if (!this->hasGoods && hasGoods)
		target = 1; // 已拿取物品
	this->hasGoods = hasGoods;
	this->x = x;
	this->y = y;
}

void Robot::assignTask(const vector<int>& moves, int target, int atHarbor, int goalHarbor)
{
	this->moves = moves;
	this->curMoveCount = 0;
	this->target = target;
	this->atHarbor = atHarbor;
	this->goalHarbor = goalHarbor;
}

pair<int,vector<int>> Robot::moveOneStep(int collisionMap[LEN][LEN])
{
	vector<int> collisions;
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
			//collisionMap[x][y] |= (id + 1) << 16; // 锁定位置
			collisionMap[x][y] = (collisionMap[x][y] & 0xFFFF) | ((id + 1) << 16);
			return make_pair(-1, collisions);
		}
		else
		{ // 不能停止，寻找一个方向避让
			// 随机构建一个0到3的数组
			int a[4] = { 0,1,2,3 };
			// 随机打乱数组
			for (int i = 0; i < 4; i++)
			{
				int j = rand() % 4;
				swap(a[i], a[j]);
			}
			int idx;
			for (idx = 0; idx < 4; idx++) // 遍历周围位置
			{
				int i = a[idx];
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
					// collisionMap[avoidPos.x][avoidPos.y] |= (id + 1) << 16; // 锁定位置
					collisionMap[avoidPos.x][avoidPos.y] = (collisionMap[avoidPos.x][avoidPos.y] & 0xFFFF) | ((id + 1) << 16);
					return make_pair(i, collisions); // 这里直接return了，如果不直接return需要break
				}
			}
			if (idx == 4) // 无法避让
			{ // 强制前进，发出冲突机器人编号

				if ((collisionMap[expectedPos.x][expectedPos.y] >> 16) != 0)
				{
					collisions.push_back((collisionMap[expectedPos.x][expectedPos.y] >> 16) - 1);
				}
				if (expectedMove != -1
					&&
					(collisionMap[expectedPos.x][expectedPos.y] & 0xffff) != 0
					&&
					(collisionMap[x][y] >> 16) == (collisionMap[expectedPos.x][expectedPos.y] & 0xffff))
				{
					collisions.push_back((collisionMap[x][y] >> 16) - 1);
				}
				//collisionMap[expectedPos.x][expectedPos.y] |= (id + 1) << 16; // 锁定位置
				collisionMap[expectedPos.x][expectedPos.y] = (collisionMap[expectedPos.x][expectedPos.y] & 0xFFFF) | ((id + 1) << 16);
				return make_pair(expectedMove, collisions);
			}
		}
	}
	else
	{
		// 锁定该位置，把高16位设置为这个机器人id+1
		//collisionMap[expectedPos.x][expectedPos.y] |= (id + 1) << 16; // 锁定位置
		collisionMap[expectedPos.x][expectedPos.y] = (collisionMap[expectedPos.x][expectedPos.y] & 0xFFFF) | ((id + 1) << 16);
		return make_pair(expectedMove, collisions);
	}

	return make_pair(-1, collisions); // 不会到达此句
	
}

void Robot::redoOneStep(int collisionMap[LEN][LEN], int originMove)
{
	Coord originPos = Coord(x, y) + originMove;
	if (collisionMap[originPos.x][originPos.y] >> 16 == id + 1)
		collisionMap[originPos.x][originPos.y] &= 0x0000ffff;
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
		// 第一种情况，在港口但找不到新的货了，此时机器人已经完成了target切换，这里继续将target保持为-1（实际上没有必要，但不影响）
		// 第二种情况，到了货物的地方但没有拿到货，target切换无效，在此处完成target=1，让机器人去港口
		// 第三种情况，机器人初始空手去港口或者第二种情况去港口，这里强制将target切换为-1让机器人去找货物
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
