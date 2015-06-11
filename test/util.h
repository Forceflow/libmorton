#ifndef UTIL_H_
#define UTIL_H_

#include <ctime>

using namespace std;

// Timer struct for easy timing
struct Timer {
	clock_t Start;
	clock_t Elapsed;
	Timer(){
		Elapsed = 0;
		Start = clock();
	}
	void reset(){
		Start = clock();
	}
	void resetTotal(){
		Elapsed = 0;
	}
	void start(){
		Start = clock();
	}
	void stop(){
		clock_t End = clock();
		Elapsed = Elapsed + (End - Start);
	}
	double getTotalTimeMs() const{
		return ((double)Elapsed) / ((double)CLOCKS_PER_SEC / 1000.0f);
	}
};

#endif