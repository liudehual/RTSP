#include "random.h"
#include <unistd.h>
#include <time.h>

Random::Random()
{
	SetSeed(PickSeed());
}

Random::Random(uint32_t seed)
{
	SetSeed(seed);
}

Random::~Random()
{
}

void Random::SetSeed(uint32_t seed)
{
	state = ((uint64_t)seed) << 16 | 0x330EULL;
}

uint32_t Random::PickSeed()
{
	uint32_t x;
	x = (uint32_t)getpid();
	x += (uint32_t)time(0);
	x += (uint32_t)clock();
	x ^= (uint32_t)((uint8_t *)this - (uint8_t *)0);
	return x;
}

uint8_t Random::GetRandom8()
{
	uint32_t x =  ((GetRandom32() >> 24)&0xff);

	return (uint8_t)x;
}

uint16_t Random::GetRandom16()
{
	uint32_t x =  ((GetRandom32() >> 16)&0xffff);

	return (uint16_t)x;
}

uint32_t Random::GetRandom32()
{
	state = ((0x5DEECE66DULL*state) + 0xBULL)&0x0000ffffffffffffULL;
	uint32_t x = (uint32_t)((state>>16)&0xffffffffULL);
	return x;
}


double Random::GetRandomDouble()
{
	state = ((0x5DEECE66DULL*state) + 0xBULL)&0x0000ffffffffffffULL;
	int64_t x = (int64_t)state;
	double y = 3.552713678800500929355621337890625e-15 * (double)x;
	return y;
}
