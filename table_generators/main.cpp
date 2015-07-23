#include <iostream>
#include "../libmorton/include/morton.h"
#include "../libmorton/include/morton_alternatives.h"

using namespace std;

void generateMortonToCoordinateTables(){
	//generate 9-bit morton to X table
	printf("\n X TABLE \n");
	for (size_t i = 0; i < 512; i++){
		unsigned int result_x = getThirdBits(i);
		if (i % 16 == 0){ printf("\n"); }
		printf("%u,", result_x);
	}
	//generate 9-bit morton to Y table
	printf("\n Y TABLE \n");
	for (size_t i = 0; i < 512; i++){
		unsigned int result_y = getThirdBits(i >> 1);
		if (i % 16 == 0){ printf("\n"); }
		printf("%u,", result_y);
	}
	//generate 9-bit morton to Z table
	printf("\n Z TABLE \n");
	for (size_t i = 0; i < 512; i++){
		unsigned int result_z = getThirdBits(i >> 2);
		if (i % 16 == 0){ printf("\n"); }
		printf("%u,", result_z);
	}
}

int main(int argc, char *argv[]) {
	generateMortonToCoordinateTables();
}