#include "defines.h"
using namespace std;

int main()
{
    cerr<<"Hello, World!"<<endl;
    Scheduler scheduler;
    while (scheduler.NextFrame())
    {
		scheduler.Update();
	}

    return 0;
}
