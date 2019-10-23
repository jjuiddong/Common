//
// 2018-06-17, jjuiddong
// Simple Memory Pool using pointer vector
// Object Thread Pool, Thread Safe
//
#pragma once


namespace common
{
	// tricky code, for cThread memorypool
	class iMemoryPool3Destructor
	{
	public:
		virtual bool Free(void* p) = 0;
	};


	template<class T, size_t Size=16>
	class cMemoryPool3 : public iMemoryPool3Destructor
	{
	public:
		cMemoryPool3();
		virtual ~cMemoryPool3();

		T* Alloc();
		virtual bool Free(void *p) override;
		void Clear();


	public:
		CriticalSection m_cs;
		vector<T*> m_freePtrs;
		vector<T*> m_allocPtrs;
	};


	template<class T, size_t Size = 16>
	cMemoryPool3<T, Size>::cMemoryPool3() {
		for (int i = 0; i < Size; ++i)
			m_freePtrs.push_back(new T());
	}

	template<class T, size_t Size = 16>
	cMemoryPool3<T, Size>::~cMemoryPool3() {
		Clear();
	}

	template<class T, size_t Size = 16>
	T* cMemoryPool3<T, Size>::Alloc() {
		AutoCSLock cs(m_cs);

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

	template<class T, size_t Size = 16>
	bool cMemoryPool3<T, Size>::Free(void* p) {
		AutoCSLock cs(m_cs);
		if (!removevector(m_allocPtrs, (T*)p))
		{
			assert(0);
			return false;
		}
		m_freePtrs.push_back((T*)p);
		return false;
	}

	template<class T, size_t Size = 16>
	void cMemoryPool3<T, Size>::Clear() {
		AutoCSLock cs(m_cs);
		for (auto &ptr : m_freePtrs)
			delete ptr;
		m_freePtrs.clear();

		for (auto &ptr : m_allocPtrs)
			delete ptr;
		m_allocPtrs.clear();
	}

}
