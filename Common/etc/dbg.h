//
// 2017-12-21, jjuiddong
// add log thread
//
#pragma once


namespace common { namespace dbg
{
	void Print( const std::string &str );
	void Print( const char* fmt, ...);

	void Log(const char* fmt, ...);
	void Log2(const char *fileName, const char* fmt, ...);
	void ErrLog(const char* fmt, ...);

	// multithreading log
	void Logp(const char* fmt, ...);
	void ErrLogp(const char* fmt, ...);

	void RemoveLog();
	void RemoveLog2(const char *fileName);
	void RemoveErrLog();
	void TerminateLogThread();

}}


extern common::cThread g_logThread;
