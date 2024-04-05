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
		target = -1; // ���ڸۿڷ�����Ʒ
	if (!this->hasGoods && hasGoods)
		target = 1; // ����ȡ��Ʒ
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
			//collisionMap[x][y] |= (id + 1) << 16; // ����λ��
			collisionMap[x][y] = (collisionMap[x][y] & 0xFFFF) | ((id + 1) << 16);
			return make_pair(-1, collisions);
		}
		else
		{ // ����ֹͣ��Ѱ��һ���������
			// �������һ��0��3������
			int a[4] = { 0,1,2,3 };
			// �����������
			for (int i = 0; i < 4; i++)
			{
				int j = rand() % 4;
				swap(a[i], a[j]);
			}
			int idx;
			for (idx = 0; idx < 4; idx++) // ������Χλ��
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
					// collisionMap[avoidPos.x][avoidPos.y] |= (id + 1) << 16; // ����λ��
					collisionMap[avoidPos.x][avoidPos.y] = (collisionMap[avoidPos.x][avoidPos.y] & 0xFFFF) | ((id + 1) << 16);
					return make_pair(i, collisions); // ����ֱ��return�ˣ������ֱ��return��Ҫbreak
				}
			}
			if (idx == 4) // �޷�����
			{ // ǿ��ǰ����������ͻ�����˱��

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
				//collisionMap[expectedPos.x][expectedPos.y] |= (id + 1) << 16; // ����λ��
				collisionMap[expectedPos.x][expectedPos.y] = (collisionMap[expectedPos.x][expectedPos.y] & 0xFFFF) | ((id + 1) << 16);
				return make_pair(expectedMove, collisions);
			}
		}
	}
	else
	{
		// ������λ�ã��Ѹ�16λ����Ϊ���������id+1
		//collisionMap[expectedPos.x][expectedPos.y] |= (id + 1) << 16; // ����λ��
		collisionMap[expectedPos.x][expectedPos.y] = (collisionMap[expectedPos.x][expectedPos.y] & 0xFFFF) | ((id + 1) << 16);
		return make_pair(expectedMove, collisions);
	}

	return make_pair(-1, collisions); // ���ᵽ��˾�
	
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
		// ��һ��������ڸۿڵ��Ҳ����µĻ��ˣ���ʱ�������Ѿ������target�л������������target����Ϊ-1��ʵ����û�б�Ҫ������Ӱ�죩
		// �ڶ�����������˻���ĵط���û���õ�����target�л���Ч���ڴ˴����target=1���û�����ȥ�ۿ�
		// ����������������˳�ʼ����ȥ�ۿڻ��ߵڶ������ȥ�ۿڣ�����ǿ�ƽ�target�л�Ϊ-1�û�����ȥ�һ���
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
