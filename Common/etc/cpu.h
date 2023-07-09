//
// 2023-07-05, jjuiddong
// check cpu, memory usage
//	- specific executable program
//
#pragma once


namespace common
{

	class cCpu
	{
	public:
		cCpu();
		virtual ~cCpu();

		bool Init(const string& exeFileName);
		int GetCpuUsage();
		int GetMemoryUsage();
		void Clear();


	protected:
		HANDLE FindProcessHandle(const string& exeFileName);


	public:
		HANDLE m_handle; // process handle
		ULARGE_INTEGER m_lastCPU;
		ULARGE_INTEGER m_lastSysCPU;
		ULARGE_INTEGER m_lastUserCPU;
		int m_numProcessors;
	};

}
