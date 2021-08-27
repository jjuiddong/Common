
#include "stdafx.h"
#include "genid.h"


namespace common
{
	std::atomic_int g_seedId = 10000;
}

using namespace common;


// set seed id
void common::SetSeedId(const int seed)
{
	g_seedId = seed;
}


// return unique integer id
int common::GenerateId()
{
	return g_seedId++;
}
