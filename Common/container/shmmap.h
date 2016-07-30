//
// 2016-04-18, jjuiddong
// 공유메모리에서 동작하는 map 컨테이너
//
#pragma once


#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/managed_windows_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <iostream> 
#include <string>
#include <boost/unordered_map.hpp>     //boost::unordered_map
#include <functional>  //std::equal_to
#include <boost/functional/hash.hpp>   //boost::hash


namespace common
{
	//using namespace boost::interprocess;

	typedef boost::interprocess::managed_shared_memory::segment_manager	segment_manager_t;
	typedef boost::interprocess::allocator<char, segment_manager_t>	char_allocator;
	typedef boost::interprocess::basic_string<char, std::char_traits<char>, char_allocator>		shm_string;

	
	template<class T>
	class cShmMap
	{
	public:
		typedef std::pair<const shm_string, T>	shm_map_value_type;
		typedef boost::interprocess::allocator<shm_map_value_type, segment_manager_t>	shm_map_type_allocator;
		typedef boost::unordered_map< shm_string, T,
			boost::hash<shm_string>, std::equal_to<shm_string>,
			shm_map_type_allocator>	shm_complex_map;

		typedef typename shm_complex_map::iterator iterator;
		typedef typename shm_complex_map::const_iterator const_iterator;


	public:
		cShmMap() : m_map(NULL), m_segment(NULL) {
		}

		virtual ~cShmMap() {
			if (m_segment)
				delete m_segment;
		}

		bool Init(const std::string &shmName)
		{
			using namespace boost::interprocess;
			shared_memory_object::remove(shmName.c_str());
			m_segment = new managed_shared_memory(open_or_create, shmName.c_str(), 65536);
			m_char_alloc_inst = new char_allocator(m_segment->get_segment_manager());

			m_map = m_segment->construct<shm_complex_map>("cShmMap")
				(3, boost::hash<shm_string>(), std::equal_to<shm_string>(),
					m_segment->get_allocator<shm_map_value_type>());
		
			return true;
		}

		bool Open(const std::string &shmName)
		{
			using namespace boost::interprocess;
			//shared_memory_object::remove(shmName.c_str());
			m_segment = new managed_shared_memory(open_or_create, shmName.c_str(), 65536);
			m_char_alloc_inst = new char_allocator(m_segment->get_segment_manager());

			m_map = m_segment->find<shm_complex_map>("cShmMap").first;
			if (!m_map)
				return false;
			return true;
		}

		void Remove(const std::string &shmName)
		{
			using namespace boost::interprocess;
			shared_memory_object::remove(shmName.c_str());
		}

		void Insert(const std::string &key, const T &value)
		{
			using namespace boost::interprocess;
			shm_string key_object(key.c_str(), *m_char_alloc_inst);
			shm_map_value_type v(key_object, value);
			m_map->insert(v);
			//(*m_map)[key_object] = value;
		}

  		auto find(const std::string &key)
  		{
			using namespace boost::interprocess;
			shm_string key_object(key.c_str(), *m_char_alloc_inst);
			return m_map->find(key_object);
  		}

		auto begin() { return m_map->begin(); }
 		auto end() { return m_map->end(); }
		const_iterator begin() const { return m_map->begin(); }
		const_iterator cbegin() const { return m_map->cbegin(); }
		const_iterator cend() const { return m_map->cend(); }
		size_t size() { return m_map->size(); }
		void clear() { m_map->clear(); }

		T& operator[] (const std::string &key)
		{
			using namespace boost::interprocess;
			shm_string key_object(key.c_str(), *m_char_alloc_inst);
			return (*m_map)[key_object];
		}


	public:
 		shm_complex_map *m_map;
		boost::interprocess::managed_shared_memory *m_segment;
		char_allocator *m_char_alloc_inst;
	};

}
