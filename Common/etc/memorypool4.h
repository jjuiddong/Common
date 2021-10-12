//
// 2019-03-08, jjuiddong
// Simple Memory Pool using pointer vector
//  - static memorypool variable
//  - Thread Safe
// inheritance this class, if need recycle memory
//
#pragma once


namespace common
{

	template<class T>
	class cMemoryPool4
	{
	public:
		cMemoryPool4() {}
		virtual ~cMemoryPool4() {}

		__forceinline void* operator new (size_t size)
		{
			return Alloc();
		}
		__forceinline void* operator new (size_t size, char *attachTypeName)
		{
			return Alloc();
		}
		__forceinline void* operator new[](size_t size)
		{
			assert(0);
			return NULL;
		}

		// Debug new
		__forceinline void* operator new (size_t size, char* lpszFileName, int nLine)
		{
			return Alloc();
		}
		__forceinline void* operator new(size_t size, const char* handle, const char * filename, int line)
		{
			return Alloc();
		}
		// Debug new
		__forceinline void* operator new[](size_t size, char* lpszFileName, int nLine)
		{
			assert(0);
			return NULL;
		}


		__forceinline void operator delete (void *ptr)
		{
			Free(ptr);
		}
		__forceinline void operator delete (void *ptr, char* lpszFileName, int nLine)
		{
			Free(ptr);
		}
		__forceinline void operator delete (void *ptr, char *attachTypeName)
		{
			Free(ptr);
		}
		__forceinline void operator delete[](void *ptr)
		{
			assert(0);
		}
		__forceinline void	operator delete (void* ptr, const char*, const char *, int)
		{
			Free(ptr);
		}

		static T* Alloc();
		static bool Free(void *p);
		static void Clear();


	public:
		static CriticalSection s_cs;
		static set<T*> s_freePtrs;
		static set<T*> s_allocPtrs;
		static boost::pool<> m_pool;
	};

	template<class T>
	CriticalSection cMemoryPool4<T>::s_cs;

	template<class T>
	set<T*> cMemoryPool4<T>::s_freePtrs;

	template<class T>
	set<T*> cMemoryPool4<T>::s_allocPtrs;

	template<class T>
	boost::pool<> cMemoryPool4<T>::m_pool(sizeof(T));


	template<class T>
	T* cMemoryPool4<T>::Alloc() {
		AutoCSLock cs(s_cs);

		T *p = NULL;
		if (s_freePtrs.empty())
		{
			p = (T*)m_pool.malloc();
		}
		else
		{
			p = *s_freePtrs.begin();
			s_freePtrs.erase(p);
		}

		if (p)
		{
			s_allocPtrs.insert(p);
		}
		return p;
	}

	template<class T>
	bool cMemoryPool4<T>::Free(void* p) {
		AutoCSLock cs(s_cs);
		s_allocPtrs.erase((T*)p);
		s_freePtrs.insert((T*)p);
		return false;
	}

	template<class T>
	void cMemoryPool4<T>::Clear() {
		AutoCSLock cs(s_cs);
		for (auto &ptr : s_freePtrs)
			m_pool.free(ptr);
		s_freePtrs.clear();

		for (auto &ptr : s_allocPtrs)
			m_pool.free(ptr);
		s_allocPtrs.clear();
	}

}
