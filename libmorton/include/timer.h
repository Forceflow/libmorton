#pragma once

#if _MSC_VER
#include <Windows.h>
#elif __GNUC__
#include <ctime>
#include <chrono>
#endif

using namespace std;
using namespace std::chrono;

#if _MSC_VER
struct Timer {
	double PCFreq = 0.0;
	double elapsed_time_milliseconds = 0.0;
	LARGE_INTEGER start_time;
	LARGE_INTEGER time;

	void init() {
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		PCFreq = double(li.QuadPart) / 1000.0;
	}

	void start() {
		QueryPerformanceCounter(&start_time);
	}

	void stop() {
		QueryPerformanceCounter(&time);
		elapsed_time_milliseconds += double((time.QuadPart - start_time.QuadPart) / PCFreq);
	}
};
#elif __GNUC__
struct Timer {
	double elapsed_time_milliseconds = 0;
	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;

	void init() {
	}

	void start() {
		t1 = high_resolution_clock::now();
	}

	void stop() {
		t2 = high_resolution_clock::now();
		elapsed_time_milliseconds += std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	}
};
#endif