
#include "stdafx.h"
#include "shmmem.h"

using namespace common;


cShmmem::cShmmem() 
	: m_memPtr(NULL)
	, m_memoryByteSyze(0)
{
}

cShmmem::~cShmmem()
{
}


// 공유메모리 생성
bool cShmmem::Init(const std::string &sharedMemoryName, const int memoryByteSize)
{
	using namespace boost::interprocess;

	m_memoryByteSyze = memoryByteSize;

	try
	{
		m_sharedmem = windows_shared_memory(open_or_create, sharedMemoryName.c_str(), read_write, memoryByteSize);
		m_mmap = mapped_region(m_sharedmem, read_write, 0, memoryByteSize);
		m_memPtr = static_cast<BYTE*>(m_mmap.get_address());
	}
	catch (interprocess_exception&e)
	{
		std::stringstream ss;
		ss << e.what() << std::endl;
		ss << "Shared Memory Error!!";
		dbg::ErrLog(ss.str().c_str());
		//::AfxMessageBox(str2wstr(ss.str()).c_str());
		m_memPtr = NULL;
		m_memoryByteSyze = 0;
		return false;
	}

	return true;
}


bool cShmmem::IsOpen()
{
	return (m_memPtr != NULL);
}

