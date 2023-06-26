
#include "stdafx.h"
#include "shmmem.h"

using namespace common;


cShmmem::cShmmem() 
	: m_memPtr(nullptr)
	, m_memoryByteSyze(0)
{
}

cShmmem::~cShmmem()
{
}


// create shared memory (open or create)
bool cShmmem::Init(const std::string &sharedMemoryName, const int memoryByteSize)
{
	using namespace boost::interprocess;

	try
	{
		m_name = sharedMemoryName;
		m_memoryByteSyze = memoryByteSize;
		m_sharedmem = windows_shared_memory(open_or_create, sharedMemoryName.c_str()
			, read_write, memoryByteSize);
		m_mmap = mapped_region(m_sharedmem, read_write, 0, memoryByteSize);
		m_memPtr = static_cast<BYTE*>(m_mmap.get_address());
	}
	catch (interprocess_exception&e)
	{
		std::stringstream ss;
		ss << "Shared Memory Error!! <" << sharedMemoryName << ">, ";
		ss << e.what() << std::endl;
		dbg::ErrLog(ss.str().c_str());
		m_memPtr = nullptr;
		m_memoryByteSyze = 0;
		m_name.clear();
		return false;
	}

	return true;
}


bool cShmmem::IsOpen()
{
	return (m_memPtr != NULL);
}



bool cShmmem::Remove()
{
	boost::interprocess::shared_memory_object::remove(m_name.c_str());
	return true;
}
