#ifndef UTIL_H_
#define UTIL_H_

#include <ctime>

using namespace std;

//static uint32_t x[4096], c = 362, a = 18705;
//
//void init_rand(uint32_t seed)
//{
//	int i;
//
//	/* Initialize random seed: */
//	srand(seed);
//	for (i = 0; i < 4096; i++)
//		x[i] = rand();
//}
//
//uint32_t rand_cmwc(void)
//{
//	static uint32_t i = 4095;
//	uint64_t t;
//
//	i = (i + 1) & 4095;
//	t = a * x[i];
//	c = (t + c) >> 32;
//	x[i] = 0xffffffff - (uint32_t)t;
//
//	return x[i];
//}

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