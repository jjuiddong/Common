
#include "stdafx.h"
#include "scanner.h"
#include "dictionary.h"

using namespace tess;


tess::cScanner::cScanner()
{
}

cScanner::cScanner(const cDictionary &dict, const string &str)
{
	Init(dict, str);
}

tess::cScanner::~cScanner()
{
}


// 소문자 변환, 공백문자는 최대 1개 까지, 
// 사용하지 않는 특수 문자 제거
bool cScanner::Init(const cDictionary &dict, const string &str)
{
	string src = str;
	trim(src);
	lowerCase(src);

	m_str.clear();

	// 쓰이는 문자, 모호한 기호를 제외한 문자 제거
	for (uint i = 0; i < src.length(); ++i)
	{
		const char c = src[i];
		if ((dict.m_useChar[c]) || (dict.m_ambiguousTable[c] != 0))
			m_str += c;
	}
	
	// 공백 최대 1개
	bool isBlank = false;
	for (uint i = 0; i < m_str.length(); )
	{
		if (m_str[i] == ' ')
		{
			if (isBlank)
			{ // 공백문자를 가장뒤로 옮긴 후, 제거
				common::rotatepopvector(m_str, i);
			}
			else
			{
				++i;
				isBlank = true;
			}
		}
		else
		{
			++i;
			isBlank = false;
		}
	}

	return true;
}


bool cScanner::IsEmpty()
{
	return m_str.empty();
}
