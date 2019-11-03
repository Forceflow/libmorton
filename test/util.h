#pragma once

#include <ctime>
#include <stdint.h>
#include <stdlib.h>


static uint32_t cmwc_x[4096];
static uint32_t cmwc_c = 362;
static uint32_t cmwc_a = 18705;

void init_randcmwc(uint32_t seed)
{
	int i;
	srand(seed);
	for (i = 0; i < 4096; i++)
		cmwc_x[i] = rand();
}

uint32_t rand_cmwc(void)
{
	static uint32_t i = 4095;
	uint64_t t;

	i = (i + 1) & 4095;
	t = cmwc_a * cmwc_x[i];
	cmwc_c = (t + cmwc_c) >> 32;
	cmwc_x[i] = 0xffffffff - static_cast<uint32_t>(t);

	return cmwc_x[i];
}