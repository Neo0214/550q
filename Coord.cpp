#include "Coord.h"
#include <iostream>
std::ostream& operator<<(std::ostream& os, const Coord& c) {
	os << "(" << c.x << "," << c.y << ")";
	return os;
}
bool operator!=(const Coord& c1, const Coord& c2) {
	return c1.x != c2.x || c1.y != c2.y;
}