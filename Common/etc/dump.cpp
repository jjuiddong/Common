
#include "stdafx.h"
#include "dump.h"
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")

using namespace common;
using namespace common::dbg;

string cDump::s_name = "";


cDump::cDump()
{
}

cDump::~cDump()
{
	Clear();
}


// initialize dump class
bool cDump::Init(const string &name)
{
	s_name = name;
	::SetUnhandledExceptionFilter(ExceptionCallback);
	return true;
}


// clear
void cDump::Clear()
{
	::SetUnhandledExceptionFilter(nullptr);
}


// exceptio callback
LONG WINAPI cDump::ExceptionCallback(
	__in struct _EXCEPTION_POINTERS* exceptioninfo)
{
	if (!exceptioninfo)
		return 0;

	SYSTEMTIME st = { 0 };
	::GetLocalTime(&st);

	// fileName: YYYY-MM-DD-HH-MM-SS-MMM-<name>.dmp
	stringstream ss;
	ss << st.wYear
		<< "-" << std::setfill('0') << std::setw(2) << st.wMonth
		<< "-" << std::setfill('0') << std::setw(2) << st.wDay
		<< "-" << std::setfill('0') << std::setw(2) << st.wHour
		<< "-" << std::setfill('0') << std::setw(2) << st.wMinute
		<< "-" << std::setfill('0') << std::setw(2) << st.wSecond
		<< "-" << std::setfill('0') << std::setw(3) << st.wMilliseconds;
	const string dateTime = ss.str();
	const string fileName = dateTime + "-" + cDump::s_name + ".dmp";

	HANDLE handle = ::CreateFileA(fileName.c_str(),
		GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == handle)
		return 0; // error return

	MINIDUMP_EXCEPTION_INFORMATION ex_info = { 0 };
	ex_info.ThreadId = ::GetCurrentThreadId(); // Threae ID 설정
	ex_info.ExceptionPointers = exceptioninfo; // Exception 정보 설정
	ex_info.ClientPointers = FALSE;

	if (!::MiniDumpWriteDump(
		::GetCurrentProcess(),
		::GetCurrentProcessId(),
		handle,
		MiniDumpNormal,
		&ex_info,
		nullptr, nullptr)) 
	{
		return 0; // error return		
	}

	return 1;
}

