#include <bits/stdc++.h>
using namespace std;
#define EMPTY -4
#define SEA -3
#define WALL -2
#define ROBOT -1
#define HARBOR 0

#define ON 1
#define OFF 0

#define NO_GOODS 0
#define HAS_GOODS 1

#define MOVING 0
#define NORMAL 1
#define WAITING 2






class Map {
private:
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
    short* getMoveCommand(int harborId, int destX, int destY);
    void setGoods(int x, int y, int val, int overFrame);
    void setPoint(int x, int y, int type);
};


class Harbor {
    friend class Map;
private:
    // 固定数据
    int Id;
    int x, y;
    int time;
    int velocity;
    // 动态数据

    // 私有函数

public:
    Harbor(int _Id, int _x, int _y, int _time, int _velocity);
    Harbor();
    // 公共函数
};

class Boat {
private:
    // 固定数据
    int id;
    int capacity;
    // 动态数据
    int status;
    int pos;
    // 私有函数

public:
    Boat(int _id, int _capacity);
    Boat();
    // 公共函数
    void update(int status, int pos);
};

class Robot {
private:
    // 固定数据
	int goods;
	int x, y;
	int status;
	// 动态数据

	// 私有函数
public:
    Robot(int _x, int _y);
    Robot();
    // 公共函数
    void update(int hasGoods, int x, int y, int status);
};
class Scheduler {
private:
    // 固定数据
    Map map;
    Harbor harbor[10];
    Boat boat[5];
    Robot robot[10];
    // 动态数据
    int frame; // 当前帧号
    // 私有函数
    void writeCommand();
public:
    Scheduler();
    // 公共函数
    bool NextFrame();
    void Update();
};
