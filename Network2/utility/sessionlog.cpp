
#include "stdafx.h"
#include "sessionlog.h"

using namespace network2;


cSessionLog::cSessionLog()
{
}

cSessionLog::~cSessionLog()
{
	Clear();
}


// *.ses 파일을 읽는다.
// key = netid, value = name
// 데이타가 중복되면, 가장 나중에 추가 된 정보로 설정한다.
// - data format
//		- session-name netid
//
bool cSessionLog::Read(const char *fileName)
{
	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;

	Str128 line;
	while (ifs.getline(line.m_str, line.SIZE))
	{
		line.trim();
		if (line.empty())
			continue;

		stringstream ss(line.m_str);
		StrId name;
		netid id;
		ss >> name.m_str >> id;
		m_sessions.remove(id);
		m_sessions.insert({ id, name });
	}

	return true;
}


const StrId& cSessionLog::FindSession(const netid id)
{
	static StrId nullStr;
	auto it = m_sessions.find(id);
	if (m_sessions.end() == it)
		return nullStr;
	return it->second;
}


void cSessionLog::Clear() 
{
	m_sessions.clear();
}
