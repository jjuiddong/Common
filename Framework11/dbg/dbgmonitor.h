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
		Vector3 mousePos;
		Vector3 eyePos;
		Vector3 lookPos;
		Vector3 eyePos2D;
		Vector3 lookPos2D;
		int drawCallCount;
		int shaderDrawCall[2][10]; // 0:vtxType, 1:count

		// shared memory -> app
		bool isDbgRender;
		int dbgRenderStyle;

		//char dummy[256];
	};

	
	class cDbgMonitor 
	{
	public:
		cDbgMonitor();
		virtual ~cDbgMonitor();

		bool Create(graphic::cRenderer &renderer, const StrId &sharedMemoryName);
		void UpdateApp(graphic::cRenderer &renderer);
		void UpdateTool(graphic::cRenderer &renderer);
		void Lock();
		void Unlock();


	public:
		cMutex m_mutex;
		sSharedData *m_sharedData;
		cShmmem m_shmem; // debug Shared Memory
	};

}
