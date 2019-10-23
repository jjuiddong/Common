//
// 2017-11-01, jjuiddong
// Simple Mutex class
//
#pragma once


namespace common
{

	class cMutex
	{
	public:
		cMutex(const StrId &name) 
			: m_name(name) 
			, m_mutex(NULL) {
		}

		virtual ~cMutex() {
			if (m_mutex)
			{
				CloseHandle(m_mutex);
				m_mutex = NULL;
			}
		}

		bool Lock() {
			do 
			{
				if (m_mutex)
					WaitForSingleObject(m_mutex, INFINITE);

				if (m_mutex)
					break;
				else
					m_mutex = CreateMutexA(NULL, TRUE, m_name.c_str());

				if (!m_mutex)
					return false;

			} while (GetLastError() == ERROR_ALREADY_EXISTS);

			return true;
		}

		bool Unlock() {
			if (m_mutex)
				ReleaseMutex(m_mutex);
			return true;
		}


	public:
		HANDLE m_mutex;
		StrId m_name;
	};

}
