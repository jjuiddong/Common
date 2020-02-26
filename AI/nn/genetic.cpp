
#include "stdafx.h"
#include "genetic.h"

namespace ai
{
	void Crossover(const vector<double> &mum, const vector<double> &dad,
		OUT vector<double> &baby1, OUT vector<double> &baby2);
	void Mutate(INOUT vector<double> &chromo);
}
using namespace ai;


void ai::Crossover(const vector<double> &mum, const vector<double> &dad,
	OUT vector<double> &baby1, OUT vector<double> &baby2)
{
	const int mumWeightSize = (int)mum.size();
	const int dadWeightSize = (int)dad.size();
	if ((mumWeightSize > 0) && (mumWeightSize == dadWeightSize))
	{
		baby1.reserve(dad.size());
		baby2.reserve(dad.size());

		// crossover dadW, mumW
		int cp = randint(0, mumWeightSize - 1);
		for (int i = 0; i < cp; ++i)
		{
			baby1.push_back(mum[i]);
			baby2.push_back(dad[i]);
		}

		for (int i = cp; i < mumWeightSize; ++i)
		{
			baby1.push_back(dad[i]);
			baby2.push_back(mum[i]);
		}
	}
	else
	{
		baby1 = mum;
		baby2 = dad;
	}
}


void ai::Mutate(INOUT vector<double> &chromo)
{
	for (uint i = 0; i < chromo.size(); ++i)
	{
		if (randfloat2() < 0.1f)
			chromo[i] += (RandomClamped() * 0.3f);
	}
}


//-------------------------------------------------------------------------------
// cGeneticAlgorithm
//-------------------------------------------------------------------------------
cGeneticAlgorithm::cGeneticAlgorithm()
{
	m_genomes.reserve(100);
}

cGeneticAlgorithm::~cGeneticAlgorithm()
{
}


bool cGeneticAlgorithm::InitGenome()
{
	m_genomes.clear();
	return true;
}


bool cGeneticAlgorithm::AddGenome(const sGenome &genome)
{
	m_genomes.push_back(genome);
	return true;
}


const vector<sGenome>& cGeneticAlgorithm::GetGenomes() const
{
	return m_genomes;
}


void cGeneticAlgorithm::Epoch()
{
	const size_t population = m_genomes.size();

	//sort the population (for scaling and elitism)
	sort(m_genomes.begin(), m_genomes.end());

	//create a temporary vector to store new chromosones
	vector<sGenome> vecNewPop;
	vecNewPop.reserve(population);

	//Now to add a little elitism we shall add in some copies of the fittest genomes.
	GrabNBest(4, 1, vecNewPop);

	//now we enter the GA loop

	//repeat until a new population is generated
	while (vecNewPop.size() < population)
	{
		//grab two chromosones
		sGenome mum = GetChromoRoulette();
		sGenome dad = GetChromoRoulette();

		//create some offspring via crossover
		vector<double> baby1, baby2;

		Crossover(mum.chromo, dad.chromo, baby1, baby2);

		//now we mutate
		Mutate(baby1);
		Mutate(baby2);

		//now copy into vecNewPop population
		vecNewPop.push_back(sGenome(baby1, 0));
		vecNewPop.push_back(sGenome(baby2, 0));
	}

	m_genomes = vecNewPop;
}


sGenome cGeneticAlgorithm::GetChromoRoulette()
{
	const int idx = (int)(m_genomes.size() * common::randfloat2());
	return m_genomes[idx];
}


//	This works like an advanced form of elitism by inserting NumCopies
//  copies of the NBest most fittest genomes into a population vector
void cGeneticAlgorithm::GrabNBest(int NBest, const int NumCopies, OUT vector<sGenome> &out)
{
	const int size = m_genomes.size();
	if (NBest > size)
		NBest = size;
	//add the required amount of copies of the n most fittest 
	//to the supplied vector
	while (NBest--)
	{
		for (int i = 0; i < NumCopies; ++i)
		{
			out.push_back(m_genomes[(size - 1) - NBest]);
		}
	}
}
