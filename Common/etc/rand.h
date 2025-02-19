#pragma once

#include <time.h>    // time()


namespace common
{

	inline void initrand()
	{
		srand((unsigned)time(NULL) );
	}


	//return a random integer between x and y
	inline int randint(int x, int y) 
	{ 
		const int p = (y - x + 1) + x;
		RETV(p <= 0, x);
		return rand() % p;
	}

	//returns a random bool
	// percentageOfTrue : 0 ~ 1
	inline bool randbool(const float percentageOfTrue)
	{
		return randint(0, 100) < (int)(100.f * percentageOfTrue);
	}


	// -1 ~ +1 random float
	// 소수점 3 째 자리 까지 랜덤.
	inline float randfloat() 
	{
		return ((float)(rand() % 2000) / 1000.f) - 1.f;
	}

	//return 0 ~ 1
	inline double randfloat2() 
	{ 
		return (double)rand() / (RAND_MAX + 1.0f); 
	}

	//return 0 ~ 1
	inline float randfloat3()
	{
		return ((float)rand() / (float)(RAND_MAX + 1.0f));
	}

	// 0 ~ +1 random float
	// 소수점 3 째 자리 까지 랜덤.
	inline float randfloatpositive()
	{
		return ((float)(rand() % 1000) / 1000.f);
	}

	//returns a random float in the range -1 < n < 1
	inline double RandomClamped() 
	{ 
		return randfloat2() - randfloat2(); 
	}

}
