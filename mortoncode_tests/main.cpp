#include "morton.h"
#include <iostream>
#include <iomanip>
#include "util.h"

using namespace std;

#define MAX 256

int main(int argc, char *argv[]) {

	Timer morton_for;
	morton_for.reset();
	morton_for.start();
	for(size_t i = 0; i < MAX; i++){
		for(size_t j = 0; j < MAX; j++){
			for(size_t k = 0; k < MAX; k++){
				mortonEncode_for(i,j,k) ;
			}
		}
	}
	morton_for.stop();

	Timer morton_magicbits;
	morton_magicbits.reset();
	morton_magicbits.start();
	for(size_t i = 0; i < MAX; i++){
		for(size_t j = 0; j < MAX; j++){
			for(size_t k = 0; k < MAX; k++){
				mortonEncode_magicbits(i,j,k) ;
			}
		}
	}
	morton_magicbits.stop();

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

	cout << "For loop: " << morton_for.getTotalTimeSeconds() << "s" << endl;
	cout << "Magic bits: " << morton_magicbits.getTotalTimeSeconds() << "s"<< endl;
	cout << "LUT: " << morton_LUT.getTotalTimeSeconds() << "s" << endl;

	//cout << "0x" << setw(8) << setfill('0') << right << hex << splitBy3(212);
}
	