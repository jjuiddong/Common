//
// 2016-07-31, jjuiddong
// 문자열 하나씩 읽어서 변환하는 기능을 가짐.
//
// 2016-08-27
//		- 모호한 문자 판별 제거
//
#pragma once


namespace tess
{

	class cScanner
	{
	public:
		cScanner();
		cScanner(const string &str);
		virtual ~cScanner();
		bool Init(const string &str);
		bool IsEmpty();


	public:
		string m_str;
	};

}