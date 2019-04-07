//------------------------------------------------------------------------
// Name:    VectorMap.h
// Author:  jjuiddong
// Date:    2/28/2013
// 
// Map 과 Vector를 합친 자료구조다.
// Fast Access 때는 Map을 이용하고, 전체를 탐색할 때는 Vector를 이용한다.
// 
// Type은 유니크한 데이터여야 한다.
//------------------------------------------------------------------------
#pragma once

#include "VectorHelper.h"

namespace common
{

	template <class KeyType, class Type>
	class VectorMap
	{
	public:
		typedef std::map<KeyType, Type> MapType;
		typedef std::vector<Type> VectorType;
		typedef typename MapType::iterator iterator;
		typedef typename MapType::value_type value_type;
		typedef typename VectorType::iterator viterator;

		VectorMap() {}
		VectorMap(int reserved) : m_seq(reserved) {}

		bool insert(const value_type &vt)
		{
			// insert map
			iterator it = m_map.find(vt.first);
			if (m_map.end() != it)
				return false; // Already Exist
			m_map.insert( vt );

			// insert vector
			putvector(m_seq, m_map.size() - 1, vt.second);
			return true;
		}

		// erase and removevector
		bool remove(const KeyType &key)
		{
			iterator it = m_map.find(key);
			if (m_map.end() == it)
				return false; // not found

			removevector(m_seq, it->second);
			m_map.erase(it);
			return true;
		}

		// erase and popvector
		bool remove2(const KeyType &key)
		{
			iterator it = m_map.find(key);
			if (m_map.end() == it)
				return false; // not found

			popvector2(m_seq, it->second);
			m_map.erase(it);
			return true;
		}


		// Type 이 유니크할 때만 사용할 수 있는 함수다.
		bool removebytype(const Type &ty)
		{
			iterator it = m_map.begin();
			while (m_map.end() != it)
			{
				if (ty == it->second)
				{
					removevector(m_seq, it->second);
					m_map.erase(it);
					return true;
				}
			}
			return false; // not found
		}

		bool empty() const
		{
			return m_map.empty();
		}

		void clear()
		{
			m_map.clear();
			m_seq.clear();
		}

		size_t size() const
		{
			return m_map.size();
		}

		void reserve(unsigned int size)
		{
			m_seq.reserve(size);
		}

		iterator find(const KeyType &key) { return m_map.find(key); }
		iterator begin() { return m_map.begin(); }
		iterator end() { return m_map.end(); }

	public:
		MapType	m_map;
		VectorType m_seq;
	};

}
