#include "../../libmorton/include/morton.h"


// Generate a decode tables for 3D morton code
// how_many_bits should be a multiple of three
void generate3DDecodeTables(size_t how_many_bits, uint8_t* x_table, uint8_t* y_table, uint8_t* z_table, bool print_tables){
	// how many items we going to make need?
	size_t total = how_many_bits << 2;
	x_table = (uint8_t*) malloc(total * sizeof(uint8_t));
	y_table = (uint8_t*) malloc(total * sizeof(uint8_t));
	z_table = (uint8_t*) malloc(total * sizeof(uint8_t));

	if (print_tables){
		printf("Generating tables for %llu bits, each table will contain %llu entries at 8 bit per entry. n", how_many_bits, total);
	}

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