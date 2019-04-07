//
// 2018-12-03, jjuiddong
// data store and file log
//
// class T must implements WriteToString() function
//	- int WriteToString(OUT char *str, const int maxSize, const bool isTerminalMark = true);
//
#pragma once


namespace common
{

	template<class T, size_t MAX_SIZE>
	class cFileLogger
	{
	public:
		cFileLogger();
		virtual ~cFileLogger();

		bool Create(const char *fileName, const bool isMultiThread = true);
		bool Add(const T &t);
		void Clear();
		void ClearThread();


	public:
		StrPath m_fileName;
		bool m_isMultiThread;
		cCircularQueue<T, MAX_SIZE> m_q;
		cCircularQueue<Str512, MAX_SIZE> m_strQ;
		static cThread s_thread;
		static int s_refCount;
	};

	template<class T, size_t MAX_SIZE>
	cThread cFileLogger<T, MAX_SIZE>::s_thread("cFileLogger");

	template<class T, size_t MAX_SIZE>
	int cFileLogger<T, MAX_SIZE>::s_refCount = 0;

	//---------------------------------------------------------------------------------
	// cFileLogTask
	//---------------------------------------------------------------------------------
	class cFileLogTask : public cTask
	{
	public:
		cFileLogTask(const int id, const char *fileName, const Str512 &str)
			: cTask(id, "cFileLogTask")
			, m_fileName(fileName)
			, m_str(str)
		{
		}
		virtual ~cFileLogTask() {
		}

		virtual eRunResult::Enum Run(const double deltaSeconds) override
		{
			std::ofstream ofs(m_fileName, std::ios::app);
			if (ofs.is_open())
			{
				const string dateStr = common::GetCurrentDateTime();
				ofs << dateStr << " " << m_str.m_str;
			}

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(33ms);
			return eRunResult::END;
		}

		const char *m_fileName;
		Str512 m_str;
	};


	//---------------------------------------------------------------------------------
	// cFileLogger<T, MAX_SIZE>
	//---------------------------------------------------------------------------------
	template<class T, size_t MAX_SIZE>
	cFileLogger<T, MAX_SIZE>::cFileLogger()
		: m_isMultiThread(true)
	{
		++s_refCount;
	}

	template<class T, size_t MAX_SIZE>
	cFileLogger<T, MAX_SIZE>::~cFileLogger()
	{
		Clear();

		--s_refCount;
		if (s_refCount == 0)
			s_thread.Clear();
	}


	template<class T, size_t MAX_SIZE>
	bool cFileLogger<T, MAX_SIZE>::Create(const char *fileName
		, const bool isMultiThread //= true
	)
	{
		m_fileName = fileName;
		m_isMultiThread = isMultiThread;
		return true;
	}


	template<class T, size_t MAX_SIZE>
	bool cFileLogger<T, MAX_SIZE>::Add(const T &t)
	{
		m_q.push(t);

		Str512 str;
		const int len = t.WriteToString(str.m_str, str.SIZE, false);
		if ((int)str.SIZE > len + 1)
		{
			str.m_str[len] = '\n';
			str.m_str[len + 1] = '\0';
		}

		m_strQ.push(str);

		if (m_isMultiThread)
		{
			static int taksId = 0;
			s_thread.AddTask(new cFileLogTask(taksId++, m_fileName.c_str(), str));
			if (!s_thread.IsRun())
				s_thread.Start();
		}
		else
		{
			std::ofstream ofs(m_fileName.c_str(), std::ios::app);
			if (ofs.is_open())
			{
				const string dateStr = common::GetCurrentDateTime();
				ofs << dateStr << " " << str.m_str;
			}
		}

		return true;
	}


	template<class T, size_t MAX_SIZE>
	void cFileLogger<T, MAX_SIZE>::Clear()
	{
	}


	template<class T, size_t MAX_SIZE>
	void cFileLogger<T, MAX_SIZE>::ClearThread()
	{
		s_refCount = 0;
		s_thread.Clear();
	}

}
