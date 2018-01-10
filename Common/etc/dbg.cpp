
#include "stdafx.h"
#include "dbg.h"
#include <chrono>

using namespace common;
using namespace dbg;


//-----------------------------------------------------------------------
// Log Thread
common::cThread g_logThread;
struct sLogData
{
	int type; // 0=log, 1=error log
	Str256 str;
};
vector<sLogData> g_logStrs;
CriticalSection g_logCS;

class cLogTask : public cTask
{
public:
	cLogTask() : cTask(0, "cLogTask") {
	}
	virtual ~cLogTask() {
	}

	virtual eRunResult::Enum Run(const double deltaSeconds) override
	{
		g_logCS.Lock();
		if (!g_logStrs.empty())
		{
			std::ofstream ofs1("log.txt", std::ios::app);
			std::ofstream ofs2("errlog.txt", std::ios::app);
			for (auto &data : g_logStrs)
			{
				switch (data.type)
				{
				case 0:
					if (ofs1.is_open())
						ofs1 << data.str.c_str();
					break;
				case 1:
					if (ofs2.is_open())
						ofs2 << data.str.c_str();
					break;
				}
			}
			g_logStrs.clear();	
		}
		g_logCS.Unlock();

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(33ms);

		return eRunResult::CONTINUE;
	}
};
//-----------------------------------------------------------------------


//------------------------------------------------------------------------
// 출력창에 스트링을 출력한다.
//------------------------------------------------------------------------
void dbg::Print( const std::string &str)
{
	OutputDebugStringA(str.c_str());
	OutputDebugStringA("\n");
}


//------------------------------------------------------------------------
// 출력창에 스트링을 출력한다. (가변인자 출력)
//------------------------------------------------------------------------
void dbg::Print( const char* fmt, ...)
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );
	OutputDebugStringA(textString);
	//OutputDebugStringA("\n");
}


void dbg::Log(const char* fmt, ...)
{
	char textString[256];
	ZeroMemory(textString, sizeof(textString));

	va_list args;
	va_start(args, fmt);
	vsnprintf_s(textString, sizeof(textString)-1, _TRUNCATE, fmt, args);
	va_end(args);

	std::ofstream ofs("log.txt", std::ios::app);
	if (ofs.is_open())
		ofs << textString;
}


// log parallel thread
void dbg::Logp(const char* fmt, ...)
{
	sLogData data;
	data.type = 0;
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(data.str.m_str, sizeof(data.str.m_str) - 1, _TRUNCATE, fmt, args);
	va_end(args);

	//------------------------------------------------------------------------
	// add string to log thread
	g_logCS.Lock();
	if (g_logStrs.size() < 256)
		g_logStrs.push_back(data);
	g_logCS.Unlock();

	if (!g_logThread.IsRun())
	{
		g_logThread.AddTask(new cLogTask());
		g_logThread.Start();
	}
}


// fileName 의 파일에 로그를 남긴다.
void dbg::Log2(const char *fileName, const char* fmt, ...)
{
	char textString[256] = { '\0' };
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(textString, sizeof(textString), _TRUNCATE, fmt, args);
	va_end(args);

	FILE *fp = fopen(fileName, "a+");
	if (fp)
	{
		fputs(textString, fp);
		fclose(fp);
	}
}


void dbg::ErrLog(const char* fmt, ...)
{
	char textString[256] = { '\0' };
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(textString, sizeof(textString), _TRUNCATE, fmt, args);
	va_end(args);

	FILE *fp = fopen("errlog.txt", "a+");
	if (fp)
	{
		fputs(textString, fp);
		fclose(fp);
	}

	// 로그파일에도 에러 메세지를 저장한다.
	Log( "Error : %s", textString);
}


// errlog parallel 
void dbg::ErrLogp(const char* fmt, ...)
{
	sLogData data;
	data.type = 1;
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(data.str.m_str, sizeof(data.str.m_str), _TRUNCATE, fmt, args);
	va_end(args);

	//------------------------------------------------------------------------
	// add string to log thread
	g_logCS.Lock();
	if (g_logStrs.size() < 256)
		g_logStrs.push_back(data);
	g_logCS.Unlock();

	// 로그파일에도 에러 메세지를 저장한다.
	Logp("Error : %s", data.str.m_str);
}


void dbg::RemoveErrLog()
{
	FILE *fp = fopen("errlog.txt", "w");
	if (fp)
	{
		fputs("", fp);
		fclose(fp);
	}
}


void dbg::RemoveLog2(const char *fileName)
{
	FILE *fp = fopen(fileName, "w");
	if (fp)
	{
		fputs("", fp);
		fclose(fp);
	}
}


void dbg::RemoveLog()
{
	FILE *fp = fopen("log.txt", "w");
	if (fp)
	{
		fputs("", fp);
		fclose(fp);
	}
}

