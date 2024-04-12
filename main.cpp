#include "defines.h"
#include "Scheduler.h"
#include "args.h"
using namespace std;

int main()
{
	//cerr.rdbuf(nullptr);
	srand(randomSeed);
	Scheduler scheduler;
#ifdef TIME_DEBUG
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
#endif // 
	while (1)
	{
		//std::chrono::steady_clock::time_point startf = std::chrono::steady_clock::now();
		if (!scheduler.NextFrame())
			break;
		scheduler.Update();
		//std::chrono::steady_clock::time_point endf = std::chrono::steady_clock::now();
		//std::chrono::duration<double> time_spanf = std::chrono::duration_cast<std::chrono::duration<double>>(endf - startf);
		//double percentf = time_spanf.count() / 0.015 * 100;
		//if (percentf > 100)
		//	cerr << "Time used: " << time_spanf.count() << " " << setiosflags(ios::fixed) << setprecision(4) << percentf << "%" << endl;
	}
#ifdef TIME_DEBUG
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	double percent = time_span.count() / 15000 / 0.015 * 100;
	cerr << "Time used: " << time_span.count() / 15000 << " " << setiosflags(ios::fixed) << setprecision(4) << percent << "%" << endl;
#endif // 


	return 0;
}
