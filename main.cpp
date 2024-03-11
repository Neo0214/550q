#include "defines.h"
#include "Scheduler.h"
using namespace std;

int main()
{
    srand(0);
    Scheduler scheduler;
    while (scheduler.NextFrame())
    {
		 scheduler.Update();
	}

    return 0;
}
