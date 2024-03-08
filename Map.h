#pragma once
#include "defines.h"

#define SEA -3
#define WALL -2
#define HARBOR -1
#define EMPTY 0

class Map {
public:
    // 固定数据
    /*
    * 0 空地
    * 1 海洋
    * 2 障碍
    * 3 机器人
    * 4 港口
    */
    int point[200][200];
    // 动态数据

    // 私有函数

public:
    Map();
    // 公共函数
    /*
    * 获取最短路径指令
    * @param harborId：港口编号
    * @param destX：目的地X坐标
    * @param destY：目的地Y坐标
    * @return short*： 指令数组，0-3，上右下左
    */
    short* getMoveCommand(int srcX, int srcY, int destX, int destY);
    void setGoods(int x, int y, int val, int overFrame);
    void setPoint(int x, int y, int type);
};