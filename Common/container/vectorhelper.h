#pragma once


namespace common
{

	/**
	 @brief this function only avaible to unique ty value
	 remove ty and then rotate vector to sequence elements
	 */
	template <class Seq>
	bool removevector(Seq &seq, const typename Seq::value_type &ty)
	{
		for (size_t i=0; i < seq.size(); ++i)
		{
			if (seq[ i] == ty)
			{
				if ((seq.size()-1) > i) // elements를 회전해서 제거한다.
					std::rotate( seq.begin()+i, seq.begin()+i+1, seq.end() );
				seq.pop_back();
				return true;
			}
		}
		return false;
	}


	// elements를 회전해서 제거한다.
	template <class Seq>
	void rotatepopvector(Seq &seq, const unsigned int idx)
	{
		if ((seq.size()-1) > idx)
			std::rotate( seq.begin()+idx, seq.begin()+idx+1, seq.end() );
		seq.pop_back();
	}


	template <class Seq>
	void rotateright(Seq &seq)
	{
		if (seq.size() > 1) 
			std::rotate(seq.rbegin(), seq.rbegin() + 1, seq.rend());
	}

	template <class Seq>
	void rotateright2(Seq &seq, const unsigned int idx)
	{
		if (seq.size() > 1)
			std::rotate(seq.begin()+idx, seq.end() - 1, seq.end());
	}


	template <class T>
	void putvector(std::vector<T> &seq, size_t putIdx, const T &ty)
	{
		if (seq.size() > putIdx)
			seq[putIdx] = ty;
		else
			seq.push_back(ty);
	}


	// idx 위치의 아이템을 제거하고, 마지막에 있는 아이템을 넣는다.
	template <class Seq>
	void popvector(Seq &seq, const unsigned int idx)
	{
		if ((seq.size() - 1) == idx)
		{
			seq.pop_back();
		}
		else if (seq.size() > idx)
		{
			seq[idx] = seq[seq.size() - 1];
			seq.pop_back();
		}
	}

	template <class Seq>
	bool popvector2(Seq &seq, const typename Seq::value_type &ty)
	{
		if (seq.empty())
			return false;
		bool reval = false;
		for (int i = (int)seq.size() - 1; i >= 0; --i)
		{
			if (seq[i] == ty)
			{
				popvector(seq, i);
				reval = true;
			}
		}
		return reval;
	}

	// rotate left idx -> 0
	template <class T>
	void rotateleft(T *p, const unsigned int idx, const unsigned int size)
	{
		if (idx == 0)
			return;

		for (uint i = idx; i < size; ++i)
			p[i - idx] = p[i];

		// todo : rotation
	}

	
	// unique vector
	template <class Seq>
	void uniquevector(Seq &seq)
	{
		for (uint i = 0; i < seq.size(); ++i)
		{
			for (uint k = i + 1; k < seq.size();)
			{
				if (seq[i] == seq[k])
				{
					rotatepopvector(seq, k);
				}
				else
				{
					++k;
				}
			}
		}
	}


}
