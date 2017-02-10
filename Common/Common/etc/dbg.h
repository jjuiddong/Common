#pragma once

namespace common { namespace dbg
{
	void Print( const std::string &str );
	void Print( const char* fmt, ...);

	void Log(const char* fmt, ...);
	void Log2(const char *fileName, const char* fmt, ...);
	void ErrLog(const char* fmt, ...);

	void RemoveLog();
	void RemoveLog2(const char *fileName);
	void RemoveErrLog();
}}
