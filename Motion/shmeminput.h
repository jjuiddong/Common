//
// 2016-04-14, jjuiddong
// 공유메모리를 이용한 모션 정보 입력 처리
//
// 2016-10-26
//		- bigendian
//
#pragma once

#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>


namespace motion
{

	class cShmemInput : public cInput
	{
	public:
		cShmemInput();
		virtual ~cShmemInput();

		bool Init(const int startIndex, const int memSize, const string &sharedMemoryName, const bool isBigEndian, const string &protocolCmd,
			const string &cmd, const string &modulatorScript);
		bool Init2(const int startIndex, const int memSize, const string &sharedMemoryName, const bool isBigEndian, const string &protocolScriptFileName,
			const string &cmdScriptFileName, const string &modulatorScriptFileName);
		virtual bool Start() override;
		virtual bool Stop() override;
		virtual bool Update(const float deltaSeconds) override;


	protected:
		bool CreateSharedMem(const string &sharedMemoryName);
		void ParseSharedMem(const BYTE *buffer, const int bufferLen);


	public:
		boost::interprocess::windows_shared_memory m_sharedmem;
		boost::interprocess::mapped_region m_mmap;
		BYTE *m_memPtr;

		int m_startIndex;
		int m_memorySize;
		bool m_isBigEndian;
		cProtocolParser m_protocolParser;
		cMathParser m_cmdParser;
		mathscript::cMathInterpreter m_cmdInterpreter;
		cMotionWaveModulator m_modulator;
	};

}
