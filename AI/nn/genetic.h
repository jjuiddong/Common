//
// 2020-02-25, jjuiddong
// genetic algorithm (Mat Buckland 2002 Idea)
//
#pragma once


namespace ai
{

	struct sGenome
	{
		double fitness;
		vector<double> chromo;

		sGenome() : fitness(0) {}
		sGenome(const vector<double> &w, double f) : chromo(w), fitness(f) {}

		friend bool operator< (const sGenome& lhs, const sGenome& rhs) {
			return (lhs.fitness < rhs.fitness);
		}
		const sGenome& operator=(const sGenome &rhs) {
			if (this != &rhs)
			{
				fitness = rhs.fitness;
				chromo = rhs.chromo;
			}
			return *this;
		}
	};


	class cGeneticAlgorithm
	{
	public:
		cGeneticAlgorithm();
		virtual ~cGeneticAlgorithm();

		bool InitGenome();
		bool AddGenome(const sGenome &genome);
		const vector<sGenome>& GetGenomes() const;
		void Epoch(const uint grabBestFitCnt=4);


	protected:
		sGenome GetChromoRoulette();
		void GrabNBest(int NBest, const int NumCopies, OUT vector<sGenome> &out);


	public:
		vector<sGenome> m_genomes;
	};

}
