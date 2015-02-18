#include "../include/morton.h"
#include <iostream>
#include <iomanip>

using namespace std;

#define MAX 256

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

#pragma optimize( "", off )
int main(int argc, char *argv[]) {
	size_t total = MAX*MAX*MAX;
	cout << "Generating " << MAX << "^3 morton codes (" << total << " in total)" << endl;

	Timer morton_LUT;
	morton_LUT.reset();
	morton_LUT.start();
	for(size_t i = 0; i < MAX; i++){
		for(size_t j = 0; j < MAX; j++){
			for(size_t k = 0; k < MAX; k++){
				mortonEncode_LUT(i,j,k);
			}
		}
	}
	morton_LUT.stop();
	cout << "LUT-based method: " << morton_LUT.getTotalTimeMs() << " ms" << endl;

	Timer morton_magicbits;
	morton_magicbits.reset();
	morton_magicbits.start();
	for (size_t i = 0; i < MAX; i++){
		for (size_t j = 0; j < MAX; j++){
			for (size_t k = 0; k < MAX; k++){
				mortonEncode_magicbits(i, j, k);
			}
		}
	}
	morton_magicbits.stop();
	cout << "Magic bits-based method: " << morton_magicbits.getTotalTimeMs() << " ms" << endl;

	Timer morton_for;
	morton_for.reset();
	morton_for.start();
	for (size_t i = 0; i < MAX; i++){
		for (size_t j = 0; j < MAX; j++){
			for (size_t k = 0; k < MAX; k++){
				mortonEncode_for(i, j, k);
			}
		}
	}
	morton_for.stop();
	cout << "For-loop method: " << morton_for.getTotalTimeMs() << " ms" << endl;
}
	