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
	void popvector2(Seq &seq, const typename Seq::value_type &ty)
	{
		if (seq.empty())
			return;

		for (int i = (int)seq.size()-1; i >= 0; --i)
			if (seq[i] == ty)
				popvector(seq, i);
	}

}
