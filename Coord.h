#pragma once
#include <iostream>
struct Coord
{
	int x;
	int y;
	Coord(int x, int y) :x(x), y(y) {};
	Coord() {};
	Coord operator+(int move) {
		switch (move)
		{
		case -1:
			return Coord(x, y);
		case 0:
			return Coord(x, y + 1);
		case 1:
			return Coord(x, y - 1);
		case 2:
			return Coord(x - 1, y);
		case 3:
			return Coord(x + 1, y);
		}

		return Coord();

	}
	bool operator==(const Coord& c) const {
		return x == c.x && y == c.y;
	}
	friend std::ostream& operator<<(std::ostream& os, const Coord& c);
	friend bool operator!=(const Coord& c1, const Coord& c2);
};