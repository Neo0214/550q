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
		target = -1; // ���ڸۿڷ�����Ʒ
	if (!this->hasGoods && hasGoods)
		target = 1; // ����ȡ��Ʒ
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
	// ����ֵ��һ������ƶ����򣬵ڶ�������ͻ�Ļ�����Id
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
	// ��ͻ���

	// ��16λΪ0���������λ�ò���֮ǰĳ�������˵���һ��λ�ã�����������λ��


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
		// ���ж��ܷ�ֹͣ
		if (collisionMap[x][y] >> 16 == 0)
		{ // ����ֹͣ
			collisionMap[x][y] |= (id + 1) << 16; // ����λ��
			return make_pair(-1, -1);
		}
		else
		{ // ����ֹͣ��Ѱ��һ���������
			int i;
			for (i = 0; i < 4; i++) // ������Χλ��
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
					collisionMap[avoidPos.x][avoidPos.y] |= (id + 1) << 16; // ����λ��
					return make_pair(i, -1); // ����ֱ��return�ˣ������ֱ��return��Ҫbreak
				}
			}
			if (i == 4) // �޷�����
			{ // ǿ��ǰ����������ͻ�����˱��
				int collisionId; // ����洢����+1���id
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
				collisionMap[expectedPos.x][expectedPos.y] |= (id + 1) << 16; // ����λ��
				return make_pair(expectedMove, collisionId-1);
			}
		}
	}
	else
	{
		// ������λ�ã��Ѹ�16λ����Ϊ���������id+1
		collisionMap[expectedPos.x][expectedPos.y] |= (id + 1) << 16; // ����λ��
		return make_pair(expectedMove, -1);
	}

	return make_pair(-1, -1); // ���ᵽ��˾�
	
}

int Robot::command(int realMove)
{
	//cerr << id << "move" << realMove<<endl;
	if (realMove >= 0)
		printf("move %d %d\n", id, realMove);
	if (curMoveCount < moves.size())
	{
		if (moves[curMoveCount] == realMove) // �����ƶ�
		{
			curMoveCount++;
			if (curMoveCount == moves.size()) // ����Ŀ��λ��
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
		else // �������ƶ�����ͣ
		{
			if (realMove == -1)
				; // do nothing
			else
				moves.insert(moves.begin() + curMoveCount, reverseMove[realMove]);
		}
	}
	else
	{
		if (realMove == -1) // ������ͣ
		{
			if (atHarbor == -1)
				target = 1;
			else
				target = -1;
		}
		else // ��ǿ���ƶ�
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
