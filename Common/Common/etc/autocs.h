//
// criticalsection 객체를 자동으로 제거하는 클래스다.
//

#pragma once


class cAutoCS
{
public:
	cAutoCS(CRITICAL_SECTION &cs) : 
		m_cs(cs)
		, m_isLeave(false) 
	{ 
		EnterCriticalSection(&cs); 
	}

	virtual ~cAutoCS() 
	{ 
		if (!m_isLeave)
			LeaveCriticalSection(&m_cs); 
		m_isLeave = true;
	}

	void Enter()
	{
		if (m_isLeave)
			EnterCriticalSection(&m_cs);
		m_isLeave = false;
	}

	void Leave() 
	{ 
		LeaveCriticalSection(&m_cs); 
		m_isLeave = true;  
	}

	CRITICAL_SECTION &m_cs;
	bool m_isLeave;
};


namespace common
{
	/// Auto Lock, Unlock
	template<class T>
	class AutoLock
	{
	public:
		AutoLock(T& t) : m_t(t) { m_t.Lock(); }
		~AutoLock() { m_t.Unlock(); }
		T &m_t;
	};



	/// Critical Section auto initial and delete
	class CriticalSection
	{
	public:
		CriticalSection();
		~CriticalSection();
		void Lock();
		void Unlock();
	protected:
		CRITICAL_SECTION m_cs;
	};

	inline CriticalSection::CriticalSection() {
		InitializeCriticalSection(&m_cs);
	}
	inline CriticalSection::~CriticalSection() {
		DeleteCriticalSection(&m_cs);
	}
	inline void CriticalSection::Lock() {
		EnterCriticalSection(&m_cs);
	}
	inline void CriticalSection::Unlock() {
		LeaveCriticalSection(&m_cs);
	}


	/// auto critical section lock, unlock
	class AutoCSLock : public AutoLock<CriticalSection>
	{
	public:
		AutoCSLock(CriticalSection &cs) : AutoLock(cs) { }
	};

}
