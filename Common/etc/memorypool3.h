//
// 2018-06-17, jjuiddong
// Simple Memory Pool using pointer vector
// Object Thread Pool, Thread Safe
//
#pragma once


namespace common
{

	template<class T, size_t Size=16>
	class cMemoryPool3
	{
	public:
		cMemoryPool3();
		virtual ~cMemoryPool3();

		T* Alloc();
		bool Free(T* p);
		void Clear();


	public:
		vector<T*> m_freePtrs;
		vector<T*> m_allocPtrs;
	};


	template<class T, size_t Size = 100>
	cMemoryPool3<T, Size>::cMemoryPool3() {
		for (int i = 0; i < Size; ++i)
			m_freePtrs.push_back(new T());
	}

	template<class T, size_t Size = 100>
	cMemoryPool3<T, Size>::~cMemoryPool3() {
		Clear();
	}

	template<class T, size_t Size = 100>
	T* cMemoryPool3<T, Size>::Alloc() {
		T *p = NULL;
		if (m_freePtrs.empty())
		{
			p = new T();
		}
		else
		{
			p = m_freePtrs.back();
			m_freePtrs.pop_back();
		}

		if (p)
		{
			m_allocPtrs.push_back(p);
		}
		return p;
	}

	template<class T, size_t Size = 100>
	bool cMemoryPool3<T, Size>::Free(T* p) {
		if (!removevector(m_allocPtrs, p))
		{
			assert(0);
			return false;
		}
		m_freePtrs.push_back(p);
		return false;
	}

	template<class T, size_t Size = 100>
	void cMemoryPool3<T, Size>::Clear() {
		for (auto &ptr : m_freePtrs)
			delete ptr;
		m_freePtrs.clear();

		for (auto &ptr : m_allocPtrs)
			delete ptr;
		m_allocPtrs.clear();
	}

}
