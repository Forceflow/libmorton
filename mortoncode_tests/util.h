#ifndef UTIL_H_
#define UTIL_H_

#include <time.h>
#include <vector>
#include <sstream>

using namespace std;


// Simple timer to measure time, using CLOCKS_PER_SEC
// Should work on all platforms, sacrificing some precision.
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
	double getTotalTimeSeconds() const{
		return ((double) Elapsed)/ ((double) CLOCKS_PER_SEC);
	}
};

#endif