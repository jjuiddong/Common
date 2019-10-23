//
// 2017-12-21, jjuiddong
// add log thread
//
// 2019-02-20
//	- add classfy logc
//	- add log path
//
#pragma once


namespace common { namespace dbg
{

	void SetLogPath(const char *path);
	void SetErrLogPath(const char *path);

	void Print(const std::string &str);
	void Print(const char* fmt, ...);

	void Log(const char* fmt, ...);
	void Log2(const char *fileName, const char* fmt, ...);
	void ErrLog(const char* fmt, ...);


	// multithreading log
	void Logp(const char* fmt, ...);
	void Logp2(const char *fileName, const char* fmt, ...);
	void ErrLogp(const char* fmt, ...);

	void RemoveLog();
	void RemoveLog2(const char *fileName);
	void RemoveErrLog();
	void TerminateLogThread();


	// log classfy
	// print/log/errlog, multithread
	//
	// level 0 : printf
	//		 1 : printf + log
	//		 2 : printf + log + errlog
	//		 3 : printf + log + errlog + assertion
	void Logc(const int level, const char* fmt, ...);
	void Logc2(const int level, const char* fmt, ...);

}}

