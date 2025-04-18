//
// 2018-04-24, jjuiddong
// memory pool  using boost pool library
// Thread-Safe
//
#pragma once


#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>
#include "etc/autocs.h"


namespace common
{

	template<size_t Size>
	class cMemoryPool2
	{
	public:
		const size_t SIZE = Size;
		cMemoryPool2() : m_pool(Size) {
		}

		void* Alloc() 
		{
			AutoCSLock cs(m_cs);
			return m_pool.malloc();
		}

		void Free(void *ptr) 
		{
			AutoCSLock cs(m_cs);
			m_pool.free(ptr);
		}


	public:
		boost::pool<> m_pool;
		CriticalSection m_cs;
	};

	//template<size_t Size>
	//boost::pool<> cMemoryPool2<Size>::m_pool(Size);

	//template<size_t Size>
	//CriticalSection cMemoryPool2<Size>::m_cs;
}
