#include "morton3D_64.h"
#include <iostream>

void generate2D_EncodeLUT(size_t how_many_bits, uint_fast32_t* x_table, uint_fast32_t* y_table, uint_fast32_t* z_table, bool print_tables){
	size_t total = how_many_bits << 2;
	x_table = (uint_fast32_t*)malloc(total * sizeof(uint_fast32_t));
	y_table = (uint_fast32_t*)malloc(total * sizeof(uint_fast32_t));


}

void generate3D_EncodeLUT(size_t how_many_bits, uint_fast32_t* x_table, uint_fast32_t* y_table, uint_fast32_t* z_table, bool print_tables){
	// how many items
	size_t total = how_many_bits << 2;


}

template <typename element>
void printTable(const element* table, size_t howmany, unsigned int splitat){
	for (size_t i = 0; i < howmany; i++){
		if (i % splitat == 0){ cout << endl; }
		cout << table[i] << ", ";
	}
	cout << endl;
}

// Generate a decode tables for 3D morton code
// how_many_bits should be a multiple of three
void generate3D_DecodeLUT(size_t how_many_bits, uint_fast8_t* x_table, uint_fast8_t* y_table, uint_fast8_t* z_table, bool print_tables){
	size_t total = how_many_bits << 2;
	x_table = (uint_fast8_t*) malloc(total * sizeof(uint_fast8_t));
	y_table = (uint_fast8_t*) malloc(total * sizeof(uint_fast8_t));
	z_table = (uint_fast8_t*) malloc(total * sizeof(uint_fast8_t));

	//generate tables
	for (size_t i = 0; i < total; i++){
		x_table[i] = getThirdBits(i);
		y_table[i] = getThirdBits(i >> 1);
		z_table[i] = getThirdBits(i >> 2);
	}

	if (print_tables){
		cout << "X Table " << endl;
		printTable<uint_fast8_t>(x_table, total, 16);
		cout << "Y Table " << endl;
		printTable<uint_fast8_t>(x_table, total, 16);
		cout << "W Table " << endl;
		printTable<uint_fast8_t>(x_table, total, 16);	
	}
}