#pragma once

#include "morton2D.h"
#include "morton3D.h"
#include <iostream>

template <typename element>
void printTable(const element* table, size_t howmany, unsigned int splitat){
	for (size_t i = 0; i < howmany; i++){
		if (i % splitat == 0){ cout << endl; }
		printf("%u ,", static_cast<unsigned int>(table[i]));
	}
	cout << endl;
}

void generate2D_EncodeLUT(size_t how_many_bits, uint_fast16_t*& x_table, uint_fast16_t*& y_table, bool print_tables){
	size_t total = 1 << how_many_bits;
	x_table = (uint_fast16_t*)malloc(total * sizeof(uint_fast16_t));
	y_table = (uint_fast16_t*)malloc(total * sizeof(uint_fast16_t));

	for (uint_fast32_t i = 0; i < total; i++){
		x_table[i] = (uint_fast16_t) m2D_e_magicbits<uint_fast32_t, uint_fast16_t>(i, 0);
		y_table[i] = (uint_fast16_t)m2D_e_magicbits<uint_fast32_t, uint_fast16_t>(0, i);
	}

	if (print_tables){
		cout << "X Table " << endl;
		printTable<uint_fast32_t>(x_table, total, 8);
		cout << "Y Table " << endl;
		printTable<uint_fast32_t>(y_table, total, 8);
	}
}

void generate2D_DecodeLUT(size_t how_many_bits, uint_fast8_t*& x_table, uint_fast8_t*& y_table, bool print_tables){
	size_t total = 1 << how_many_bits;
	x_table = (uint_fast8_t*)malloc(total * sizeof(uint_fast8_t));
	y_table = (uint_fast8_t*)malloc(total * sizeof(uint_fast8_t));

	//generate tables
	for (size_t i = 0; i < total; i++) {
		m2D_d_for(i, x_table[i], y_table[i]);
	}

	if (print_tables) {
		cout << "X Table " << endl;
		printTable<uint_fast8_t>(x_table, total, 16);
		cout << "Y Table " << endl;
		printTable<uint_fast8_t>(y_table, total, 16);
	}
}

void generate3D_EncodeLUT(size_t how_many_bits, uint_fast32_t*& x_table, uint_fast32_t*& y_table, uint_fast32_t*& z_table, bool print_tables){
	// how many items
	size_t total = 1 << how_many_bits;
	x_table = (uint_fast32_t*)malloc(total * sizeof(uint_fast32_t));
	y_table = (uint_fast32_t*)malloc(total * sizeof(uint_fast32_t));
	z_table = (uint_fast32_t*)malloc(total * sizeof(uint_fast32_t));

	for (uint_fast32_t i = 0; i < total; i++){
		x_table[i] = (uint_fast32_t) m3D_e_magicbits<uint_fast32_t, uint_fast16_t>(i, 0, 0);
		y_table[i] = (uint_fast32_t) m3D_e_magicbits<uint_fast32_t, uint_fast16_t>(0, i, 0);
		z_table[i] = (uint_fast32_t) m3D_e_magicbits<uint_fast32_t, uint_fast16_t>(0, 0, i);
	}

	if (print_tables){
		cout << "X Table " << endl;
		printTable<uint_fast32_t>(x_table, total, 8);
		cout << "Y Table " << endl;
		printTable<uint_fast32_t>(y_table, total, 8);
		cout << "Z Table " << endl;
		printTable<uint_fast32_t>(z_table, total, 8);
	}
}

// Generate a decode tables for 3D morton code
// how_many_bits should be a multiple of three
void generate3D_DecodeLUT(size_t how_many_bits, uint_fast8_t*& x_table, uint_fast8_t*& y_table, uint_fast8_t*& z_table, bool print_tables){
	size_t total = 1 << how_many_bits;
	x_table = (uint_fast8_t*) malloc(total * sizeof(uint_fast8_t));
	y_table = (uint_fast8_t*) malloc(total * sizeof(uint_fast8_t));
	z_table = (uint_fast8_t*) malloc(total * sizeof(uint_fast8_t));

	//generate tables
	for (size_t i = 0; i < total; i++){
		x_table[i] = morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i);
		y_table[i] = morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i >> 1);
		z_table[i] = morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i >> 2);
	}

	if (print_tables){
		cout << "X Table " << endl;
		printTable<uint_fast8_t>(x_table, total, 16);
		cout << "Y Table " << endl;
		printTable<uint_fast8_t>(y_table, total, 16);
		cout << "Z Table " << endl;
		printTable<uint_fast8_t>(z_table, total, 16);	
	}
}
