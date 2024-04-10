#pragma once
#include "defines.h"
#include "Coord.h"

class BoatState {
public:
	int x, y;
	int direction;

	Coord getCoord() {
		return Coord(x, y);
	}
	BoatState(int x, int y, int direction) {
		this->x = x;
		this->y = y;
		this->direction = direction;
	}
	BoatState(const Coord& coord, int direction) {
		this->x = coord.x;
		this->y = coord.y;
		this->direction = direction;
	}
	BoatState() {
		this->x = 0;
		this->y = 0;
		this->direction = 0;
	}
	BoatState operator- (int value) {
		//0123右左上下
		BoatState result;
		switch (value)
		{
		case FORWARD: {
			Coord cur(x, y);
			Coord last = cur + reverseMove[direction];
			int lastDirection = direction;
			return BoatState(last, lastDirection);
		}
		case LEFTTURN: {
			Coord cur(x, y);
			int lastDirection = rightTurn[direction];
			Coord last = cur + direction + leftTurn[direction];
			return BoatState(last, lastDirection);
		}
		case RIGHTTURN: {
			Coord cur(x, y);
			int lastDirection = leftTurn[direction];
			Coord last = cur + rightTurn[direction] + rightTurn[direction];
			return BoatState(last, lastDirection);
		}
		}
		cerr << "action error" << endl;
		return BoatState();
	}
	BoatState operator+ (int value) {
		//0123右左上下
		BoatState result;
		switch (value)
		{
		case FORWARD: {
			Coord cur(x, y);
			Coord next = cur + direction;
			int nextDirection = direction;
			return BoatState(next, nextDirection);
		}
		case LEFTTURN: {
			Coord cur(x, y);
			int nextDirection = leftTurn[direction];
			Coord next = cur + direction + rightTurn[direction];
			return BoatState(next, nextDirection);
		}
		case RIGHTTURN: {
			Coord cur(x, y);
			int nextDirection = rightTurn[direction];
			Coord next = cur + direction + direction;
			return BoatState(next, nextDirection);
		}
		}
		cerr << "action error" << endl;
		return BoatState();
	}
};

