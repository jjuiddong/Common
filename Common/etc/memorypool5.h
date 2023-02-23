//
// 2013-02-21, jjuiddong
// Simple Memory Pool using pointer vector
// Thread UnSafe
//	- modify from cMemoryPool3
//
#pragma once


namespace common
{
	template<class T, size_t Size=16>
	class cMemoryPool5
	{
	public:
		cMemoryPool5();
		virtual ~cMemoryPool5();

		T* Alloc();
		bool Free(T *p);
		void Clear();


	public:
		set<T*> m_freePtrs;
		set<T*> m_allocPtrs;
	};


	template<class T, size_t Size = 16>
	cMemoryPool5<T, Size>::cMemoryPool5() {
		for (int i = 0; i < Size; ++i)
			m_freePtrs.insert(new T());
	}

	template<class T, size_t Size = 16>
	cMemoryPool5<T, Size>::~cMemoryPool5() {
		Clear();
	}

	template<class T, size_t Size = 16>
	T* cMemoryPool5<T, Size>::Alloc() {
		T *p = NULL;
		if (m_freePtrs.empty())
		{
			p = new T();
		}
		else
		{
			p = *m_freePtrs.begin();
			m_freePtrs.erase(p);
		}

		if (p)
		{
			m_allocPtrs.insert(p);
		}
		return p;
	}

	template<class T, size_t Size = 16>
	bool cMemoryPool5<T, Size>::Free(T *p) {
		if (!p) return false;
		m_allocPtrs.erase(p);
		m_freePtrs.insert(p);
		return true;
	}

	template<class T, size_t Size = 16>
	void cMemoryPool5<T, Size>::Clear() {
		for (auto &ptr : m_freePtrs)
			delete ptr;
		m_freePtrs.clear();

		for (auto &ptr : m_allocPtrs)
			delete ptr;
		m_allocPtrs.clear();
	}

}
