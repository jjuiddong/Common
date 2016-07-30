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


	template <class T>
	void putvector(std::vector<T> &seq, size_t putIdx, const T &ty)
	{
		if (seq.size() > putIdx)
			seq[putIdx] = ty;
		else
			seq.push_back(ty);
	}

}
