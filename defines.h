#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <bits/stdc++.h>

#define HARBOR_NUM 10 // �ۿ�����
#define ROBOT_NUM 1
#define LEN 200 // ��ͼ�߳�

using namespace std;

struct Coord
{
	int x;
	int y;
	Coord(int x, int y) :x(x), y(y) {};
	Coord() {};
};