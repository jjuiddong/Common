
#include "stdafx.h"
#include "sharedmemcommunication.h"

using namespace boost::interprocess;


cSharedMemoryCommunication::cSharedMemoryCommunication()
{
}

cSharedMemoryCommunication::~cSharedMemoryCommunication()
{
}


// 클래스 초기화.
bool cSharedMemoryCommunication::Init(const std::string &sharedMemoryName)
{
	try
	{
		//shared_memory_object::remove(sharedMemoryName.c_str());
		m_sharedmem = shared_memory_object(open_or_create, sharedMemoryName.c_str(), read_write);

		// setting the size of the shared memory
		m_sharedmem.truncate(512);

		// map the shared memory to current process 
		m_mmap = mapped_region(m_sharedmem, read_write, 0, 256);

		// access the mapped region using get_address
		//strcpy(static_cast<char*>(mmap.get_address()), "Hello World!\n");

	}
	catch (interprocess_exception&e)
	{
		std::cout << e.what() << std::endl;
		std::cout << "Shared Memory Error!!";
		return false;
	}

	return true;
}


// 공유 메모리 주소를 리턴한다.
void* cSharedMemoryCommunication::GetMemoryPtr()
{
	return m_mmap.get_address();
}

