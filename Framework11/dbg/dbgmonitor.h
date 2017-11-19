//
// 2017-11-15, jjuiddong
// Debug Monitor Using SharedMemory
//
#pragma once


namespace framework
{

	struct sSharedData 
	{
		// app -> shared memory
		float fps;
		double dtVal;

		// shared memory -> app
		bool isDbgRender;
		int dbgRenderStyle;

		char dummy[256];
	};

	
	class cDbgMonitor 
	{
	public:
		cDbgMonitor();
		virtual ~cDbgMonitor();

		bool Create(graphic::cRenderer &renderer, const StrId &sharedMemoryName);
		void UpdateApp(graphic::cRenderer &renderer);
		void UpdateTool(graphic::cRenderer &renderer);


	public:
		cMutex m_mutex;
		sSharedData *m_sharedData;
		cShmmem m_shmem; // debug Shared Memory
	};

}
