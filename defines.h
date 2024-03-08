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
    // �̶�����
    /*
    * 0 �յ�
    * 1 ����
    * 2 �ϰ�
    * 3 ������
    * 4 �ۿ�
    */
    int point[200][200];
    // ��̬����

    // ˽�к���
    
public:
    Map();
    // ��������
    /* 
    * ��ȡ���·��ָ��
    * @param harborId���ۿڱ��
    * @param destX��Ŀ�ĵ�X����
    * @param destY��Ŀ�ĵ�Y����
    * @return short*�� ָ�����飬0-3����������
    */
    short* getMoveCommand(int harborId, int destX, int destY);
    void setGoods(int x, int y, int val, int overFrame);
    void setPoint(int x, int y, int type);
};


class Harbor {
    friend class Map;
private:
    // �̶�����
    int Id;
    int x, y;
    int time;
    int velocity;
    // ��̬����

    // ˽�к���

public:
    Harbor(int _Id, int _x, int _y, int _time, int _velocity);
    Harbor();
    // ��������
};

class Boat {
private:
    // �̶�����
    int id;
    int capacity;
    // ��̬����
    int status;
    int pos;
    // ˽�к���

public:
    Boat(int _id, int _capacity);
    Boat();
    // ��������
    void update(int status, int pos);
};

class Robot {
private:
    // �̶�����
	int goods;
	int x, y;
	int status;
	// ��̬����

	// ˽�к���
public:
    Robot(int _x, int _y);
    Robot();
    // ��������
    void update(int hasGoods, int x, int y, int status);
};
class Scheduler {
private:
    // �̶�����
    Map map;
    Harbor harbor[10];
    Boat boat[5];
    Robot robot[10];
    // ��̬����
    int frame; // ��ǰ֡��
    // ˽�к���
    void writeCommand();
public:
    Scheduler();
    // ��������
    bool NextFrame();
    void Update();
};
