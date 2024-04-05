#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define TIME_DEBUG

#include <bits/stdc++.h>
#ifdef TIME_DEBUG
#include <chrono>
#endif // TIME_DEBUG


#define LEN 200 // 地图边长


#define EMPTY 0
#define HARBOR -1


// 地图输入数据
#define EMPTY_SPACE '.'
#define MAIN_LAND '>'
#define SEA_SPACE '*'
#define MAIN_SEA '~'
#define BLOCK '#'
#define BUY_ROBOT_SPACE 'R'
#define BUY_SHIP_SPACE 'S'
#define HARBOR_SPACE 'B'
#define LOAD_SPACE 'K'
#define OVERPASS 'C'
#define OVERPASS_MAIN 'c'
#define DELIVERY 'T'

using namespace std;
constexpr int reverseMove[4] = { 1,0,3,2 }; // 移动方向反转数组



