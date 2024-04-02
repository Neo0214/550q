#include "defines.h"
#include "Scheduler.h"
#include "args.h"
using namespace std;

int main()
{
    srand(randomSeed);
    Scheduler scheduler;
    while (scheduler.NextFrame())
    {
		 scheduler.Update();
	}
    
    return 0;
}
