
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

	static bool onlyOne = false;
	if (onlyOne) return 0; // nothing to do
	onlyOne = true;

	SYSTEMTIME st = { 0 };
	::GetLocalTime(&st);

	// fileName: YYYY-MM-DD-HH-MM-SS-MMM-<name>.dmp
	const string dateTime = common::format("%d-%d-%d-%d-%d-%d", st.wYear
		, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
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

