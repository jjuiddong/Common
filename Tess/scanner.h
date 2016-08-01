//
// 2016-07-31, jjuiddong
// 문자열 하나씩 읽어서 변환하는 기능을 가짐.
//
#pragma once


namespace tess
{

	class cDictionary;
	class cScanner
	{
	public:
		cScanner();
		cScanner(const cDictionary &dict, const string &str);
		virtual ~cScanner();
		bool Init(const cDictionary &dict, const string &str);
		bool IsEmpty();


	public:
		string m_str;
	};

}