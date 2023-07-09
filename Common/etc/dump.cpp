
#include "stdafx.h"
#include "dump.h"
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")

using namespace common;
using namespace common::dbg;

string cDump::m_fileName = "dump.dmp";


cDump::cDump()
{
}

cDump::~cDump()
{
	Clear();
}


// initialize dump class
bool cDump::Init(
	const string &dumpFileName //=""
)
{
	string fileName;
	if (dumpFileName.empty())
	{
		// fileName: YYYY-MM-DD-HH-MM-SS-MMM.dmp
		fileName = common::GetCurrentDateTime() + ".dmp";
	}
	else
	{
		fileName = dumpFileName;
	}

	m_fileName = fileName;
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
	std::string fileNname = cDump::m_fileName;

	HANDLE handle = ::CreateFileA(fileNname.c_str(),
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

