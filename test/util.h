#pragma once

#include <ctime>
#include <stdint.h>
#include <stdlib.h>

using namespace std;

static uint32_t x[4096];
static uint32_t c = 362;
static uint32_t a = 18705;

void init_randcmwc(uint32_t seed)
{
	int i;
	srand(seed);
	for (i = 0; i < 4096; i++)
		x[i] = rand();
}

uint32_t rand_cmwc(void)
{
	static uint32_t i = 4095;
	uint64_t t;

	i = (i + 1) & 4095;
	t = a * x[i];
	c = (t + c) >> 32;
	x[i] = 0xffffffff - static_cast<uint32_t>(t);

	return x[i];
}