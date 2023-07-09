
#include "stdafx.h"
#include "cpu.h"
#include <tlhelp32.h> // cpu usage
#include <psapi.h> // memory usage

using namespace common;


cCpu::cCpu()
	: m_handle(nullptr)
{
}

cCpu::~cCpu()
{
	Clear();
}


// initialize class
bool cCpu::Init(const string& exeFileName)
{
	m_handle = FindProcessHandle(exeFileName);
	if (!m_handle)
		return false; // error return

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	m_numProcessors = sysInfo.dwNumberOfProcessors;

	FILETIME ftime, fsys, fuser;
	GetSystemTimeAsFileTime(&ftime);
	memcpy(&m_lastCPU, &ftime, sizeof(FILETIME));
		
	GetProcessTimes(m_handle, &ftime, &ftime, &fsys, &fuser);
	memcpy(&m_lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&m_lastUserCPU, &fuser, sizeof(FILETIME));
	return true;
}


// find process handle
// https://stackoverflow.com/questions/865152/how-can-i-get-a-process-handle-by-its-name-in-c
// exeFileName: filename, ex) filename.exe
HANDLE cCpu::FindProcessHandle(const string& exeFileName)
{
	const wstring fileName = common::str2wstr(exeFileName);
    HANDLE retVal = nullptr;
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32FirstW(snapshot, &entry) == TRUE)
    {
        while (Process32NextW(snapshot, &entry) == TRUE)
        {
            if (_wcsicmp(entry.szExeFile, fileName.c_str()) == 0)
            {
                retVal = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                break;
            }
        }
    }
    CloseHandle(snapshot);
	return retVal;
}


// return process cpu usage (percentage 0~100)
// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
// -1 return if error
int cCpu::GetCpuUsage()
{
    RETV(!m_handle, -1);

	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(m_handle, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (double)((sys.QuadPart - m_lastSysCPU.QuadPart) +
		(user.QuadPart - m_lastUserCPU.QuadPart));
	percent /= (now.QuadPart - m_lastCPU.QuadPart);
	percent /= m_numProcessors;
	m_lastCPU = now;
	m_lastUserCPU = user;
	m_lastSysCPU = sys;

	return (int)(percent * 100);
}


// return process memory usage (1MB)
// https://stackoverflow.com/questions/866202/in-c-win32-app-how-can-i-determine-private-bytes-working-set-and-virtual-size
// -1 return if error
int cCpu::GetMemoryUsage()
{
	RETV(!m_handle, -1);

	PROCESS_MEMORY_COUNTERS_EX  pmc;
	if (!GetProcessMemoryInfo(m_handle, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
		return -1;

	const uint memSize = pmc.PrivateUsage / (1024 * 1024); // MB
	return (int)memSize;
}


// clear class
void cCpu::Clear()
{
	if (m_handle)
	{
		CloseHandle(m_handle);
		m_handle = nullptr;
	}
}
