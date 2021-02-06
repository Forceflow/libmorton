// Portable high-precision timer
// Using QueryPerformanceCounter for Win32/Win64
// And POSIX get_clock() for other platforms

#pragma once

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(__GNUC__)
#include "time.h"
#endif

#if defined(_MSC_VER)
struct Timer { 
	double pc_frequency = 0.0;
	double elapsed_time_milliseconds = 0.0;
	LARGE_INTEGER start_time = { 0 };
	LARGE_INTEGER end_time = { 0 };

	inline Timer() {
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		pc_frequency = static_cast<double>(li.QuadPart) / 1000.0;
	}

	inline void reset() {
		elapsed_time_milliseconds = 0.0;
	}

	inline void start() {
		QueryPerformanceCounter(&start_time);
	}

	inline void stop() {
		QueryPerformanceCounter(&end_time);
		elapsed_time_milliseconds += static_cast<double>((end_time.QuadPart - start_time.QuadPart) / pc_frequency);
	}
};
#else
struct Timer {
	double elapsed_time_milliseconds = 0;
	timespec t1;
	timespec t2;

	inline Timer() {
	}

	inline void start() {
		clock_gettime(CLOCK_REALTIME, &t1);
	}

	inline void stop() {
		clock_gettime(CLOCK_REALTIME, &t2);
		elapsed_time_milliseconds += (t2.tv_sec - t1.tv_sec) * 1000.0f;
		elapsed_time_milliseconds += ((float)(t2.tv_nsec - t1.tv_nsec)) / 1000000.0f;
	}
};
#endif
