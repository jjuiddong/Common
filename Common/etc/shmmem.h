// 
// 2016-04-27, jjuiddong
// Shared Memeory
//
#pragma once

#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>


namespace common
{

	class cShmmem
	{
	public:
		cShmmem();
		virtual ~cShmmem();

		bool Init(const std::string &sharedMemoryName, const int memoryByteSize = 256);
		bool Remove();
		bool IsOpen();


	public:
		string m_name; // shared memory name
		boost::interprocess::windows_shared_memory m_sharedmem;
		boost::interprocess::mapped_region m_mmap;
		BYTE *m_memPtr;
		int m_memoryByteSyze;
	};

}
