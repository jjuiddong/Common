//
// 2019-02-06, jjuiddong
// Semaphore
//
// Reference
//	- https://github.com/preshing/cpp11-on-multicore/blob/master/common/sema.h
//
#pragma once

namespace common
{

	class cSemaphore
	{
	public:
		cSemaphore(const int initialCount = 0);
		virtual ~cSemaphore();
		void Wait();
		void Signal(const int count = 1);
		void Clear();
	public:
		HANDLE m_handle;
	};

	inline cSemaphore::cSemaphore(const int initialCount //= 0
	) {
		assert(initialCount >= 0);
		m_handle = CreateSemaphore(NULL, initialCount, MAXLONG, NULL);
	}

	inline cSemaphore::~cSemaphore() {
		CloseHandle(m_handle);
	}

	inline void cSemaphore::Wait() {
		WaitForSingleObject(m_handle, INFINITE);
	}

	inline void cSemaphore::Signal(const int count //= 1
	) {
		ReleaseSemaphore(m_handle, count, NULL);
	}

	inline void cSemaphore::Clear() {
		LONG count = 0;
		do {
			ReleaseSemaphore(m_handle, 0, &count);
			if (count > 0)
				WaitForSingleObject(m_handle, INFINITE);
		} while (count > 0);
	}

}
