
#include "stdafx.h"
#include "dbg.h"
#include <chrono>


namespace common {
	namespace dbg {

		// Log Thread
		common::cWQSemaphore g_logThread;
		struct sLogData
		{
			int type; // 0:log, 1:error log, 2:log + error log, 3:others file
			Str512 str;
		};

		StrPath g_logPath = "log.txt";
		StrPath g_errLogPath = "errlog.txt";


// make sLogData from argument list
#define MAKE_LOGDATA(logData, logType, fmt) \
		{\
			logData.type = logType;\
			va_list args;\
			va_start(args, fmt);\
			vsnprintf_s(logData.str.m_str, sizeof(logData.str.m_str) - 1, _TRUNCATE, fmt, args);\
			va_end(args);\
		}

	}
}

using namespace common;
using namespace dbg;


//------------------------------------------------------------------------
// LogThread Task
class cLogTask : public cTask
				, public common::cMemoryPool4<cLogTask>
{
public:
	const char *m_fileName;
	sLogData m_logData;
	cLogTask() : cTask(0, "cLogTask") {}
	cLogTask(const sLogData &logData, const char *fileName = NULL)
		: cTask(0, "cLogTask"), m_logData(logData), m_fileName(fileName) {
	}
	virtual ~cLogTask() {
	}

	virtual eRunResult Run(const double deltaSeconds) override
	{
		const string timeStr = common::GetCurrentDateTime();

		switch (m_logData.type)
		{
		case 0:
		{
			std::ofstream ofs1(g_logPath.c_str(), std::ios::app);
			if (ofs1.is_open())
			{
				ofs1 << timeStr << " : ";
				ofs1 << m_logData.str.c_str();
			}
		}
		break;

		case 1:
		{
			std::ofstream ofs2(g_errLogPath.c_str(), std::ios::app);
			if (ofs2.is_open())
			{
				ofs2 << timeStr << " : ";
				ofs2 << m_logData.str.c_str();
			}
		}
		break;

		case 2:
		{
			std::ofstream ofs1(g_logPath.c_str(), std::ios::app);
			std::ofstream ofs2(g_errLogPath.c_str(), std::ios::app);
			if (ofs1.is_open())
			{
				ofs1 << timeStr << " : ";
				ofs1 << m_logData.str.c_str();
			}
			if (ofs2.is_open())
			{
				ofs2 << timeStr << " : ";
				ofs2 << m_logData.str.c_str();
			}
		}
		break;

		case 3:
		{
			std::ofstream ofs1(m_fileName, std::ios::app);
			if (ofs1.is_open())
			{
				ofs1 << m_logData.str.c_str();
			}
		}
		break;

		default: assert(!"LogTask Error"); break;
		}
		return eRunResult::End;
	}
};
//-----------------------------------------------------------------------


//------------------------------------------------------------------------
// Set Log File Path
// ex) "./server/"  --> "./server/log.txt"
// ex) "./client/" --> "./client/log.txt"
//------------------------------------------------------------------------
void dbg::SetLogPath(const char *path)
{
	g_logPath = StrPath(path) + g_logPath;
}


//------------------------------------------------------------------------
// Set ErrLog File Path
// ex) "./server/"  --> "./server/errlog.txt"
// ex) "./client/" --> "./client/errlog.txt"
//------------------------------------------------------------------------
void dbg::SetErrLogPath(const char *path)
{
	g_errLogPath = StrPath(path) + g_errLogPath;
}


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

	std::ofstream ofs(g_logPath.c_str(), std::ios::app);
	if (ofs.is_open())
		ofs << textString;
}


// log parallel thread
void dbg::Logp(const char* fmt, ...)
{
	sLogData data;
	MAKE_LOGDATA(data, 0, fmt);

	// add string to log thread
	g_logThread.PushTask(new cLogTask(data));
}


// log specific file, parallel thread
void dbg::Logp2(const char *fileName, const char* fmt, ...)
{
	sLogData data;
	MAKE_LOGDATA(data, 3, fmt);

	// add string to log thread
	g_logThread.PushTask(new cLogTask(data, fileName));
}


// fileName 의 파일에 로그를 남긴다.
void dbg::Log2(const char *fileName, const char* fmt, ...)
{
	sLogData data;
	MAKE_LOGDATA(data, 0, fmt);

	FILE *fp = fopen(fileName, "a+");
	if (fp)
	{
		fputs(data.str.c_str(), fp);
		fclose(fp);
	}
}


void dbg::ErrLog(const char* fmt, ...)
{
	sLogData data;
	MAKE_LOGDATA(data, 1, fmt);

	FILE *fp = fopen(g_errLogPath.c_str(), "a+");
	if (fp)
	{
		fputs(data.str.c_str(), fp);
		fclose(fp);
	}

	// 로그파일에도 에러 메세지를 저장한다.
	Log( "Error : %s", data.str.c_str());
}


// errlog parallel 
void dbg::ErrLogp(const char* fmt, ...)
{
	sLogData data;
	MAKE_LOGDATA(data, 1, fmt);

	// add string to log thread
	g_logThread.PushTask(new cLogTask(data));

	// 로그파일에도 에러 메세지를 저장한다.
	Logp("Error : %s", data.str.m_str);
}


void dbg::RemoveErrLog()
{
	FILE *fp = fopen(g_errLogPath.c_str(), "w");
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
	FILE *fp = fopen(g_logPath.c_str(), "w");
	if (fp)
	{
		fputs("", fp);
		fclose(fp);
	}
}


void dbg::TerminateLogThread()
{
	g_logThread.Clear();
}


// log classfy
// none/log/errlog, multithread
//
// level 0 : none
//		 1 : log
//		 2 : log + err log
//		 3 : log + err log + assertion
void dbg::Logc(const int level, const char* fmt, ...)
{
	sLogData data;
	MAKE_LOGDATA(data, -1, fmt);

	switch (level)
	{
	case 3:
#ifdef _DEBUG
		DebugBreak();
#endif
		assert(!"dbg::Logc()");
	case 2:
		data.type = 2;
	case 1:
		if (level == 1)
			data.type = 0;
	case 0:
		// cout 은 화면이 Freeze 현상으로 멈출수 있기 때문에 제외됨
		if (0 == level)
			return;
		break;
	default:
#ifdef _DEBUG
		DebugBreak();
#endif
		assert(!"dbg::Logc()");
		break;
	}

	//------------------------------------------------------------------------
	// add string to log thread
	if (data.type >= 0)
		g_logThread.PushTask(new cLogTask(data));
}


// log classfy
// print/log/errlog, multithread
//
// level 0 : printf
//		 1 : printf + log
//		 2 : printf + log + err log
//		 3 : printf + log + err log + assertion
void dbg::Logc2(const int level, const char* fmt, ...)
{
	sLogData data;
	MAKE_LOGDATA(data, -1, fmt);

	switch (level)
	{
	case 3:
#ifdef _DEBUG
		DebugBreak();
#endif
		assert(!"dbg::Logc2()");
	case 2:
		data.type = 2;
	case 1:
		if (level == 1)
			data.type = 0;
	case 0:
		std::cout << data.str.m_str;
		break;
	default:
#ifdef _DEBUG
		DebugBreak();
#endif
		assert(!"dbg::Logc2()");
		break;
	}

	//------------------------------------------------------------------------
	// add string to log thread
	if (data.type >= 0)
		g_logThread.PushTask(new cLogTask(data));
}
