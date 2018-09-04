#ifndef _RANDOM_H
#define _RANDOM_H
#include <stdint.h>

#define Random_2POWMIN63	1.08420217248550443400745280086994171142578125e-19

/** Interface for generating random numbers. */
class Random
{
public:
	Random();
	Random(uint32_t seed);
	virtual ~Random();

	/** Returns a random eight bit value. */
	uint8_t GetRandom8();

	/** Returns a random sixteen bit value. */
	uint16_t GetRandom16();

	/** Returns a random thirty-two bit value. */
	uint32_t GetRandom32();

	/** Returns a random number between $0.0$ and $1.0$. */
	double GetRandomDouble();
private:
	/** generate a seed for a random number generator. */
	uint32_t PickSeed();
	void SetSeed(uint32_t seed);
	uint64_t state;
};
#endif


