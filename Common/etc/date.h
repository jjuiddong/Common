//
// 2019-03-28, jjuiddong
// date time functions
//	- use boost library
//
// 2020-05-09
//	- refactoring, update comment
//
#pragma once


namespace common
{

	string GetCurrentDateTime(); // yyyy-mm-dd-hh-mm-ss-mmm
	string GetCurrentDateTime1(); // yyyy-mm-dd-hh-mm-ss
	string GetCurrentDateTime2(); // yyyymmddhhmmssmmm
	unsigned __int64 GetCurrentDateTime3();
	string GetCurrentDateTime4(); // yyyymmdd
	string GetCurrentDateTime5(); // yyyy-mm-dd hh:mm:ss
	uint64 GetCurrentDateTime6(const string &dateTime);
	
	int DateCompare(const int ymd0, const int ymd1);
	int DateCompare2(const int yyyy1, const int mm1, const int dd1
		, const int yyyy0, const int mm0, const int dd0);
	

	class cDateTime
	{
	public:
		cDateTime();
		cDateTime(const string &strDate);
		virtual ~cDateTime();

		bool Parse(const char *str);


	public:
		BYTE m_year;
		BYTE m_month;
		BYTE m_day;
		BYTE m_hours;
		BYTE m_minutes;
		BYTE m_seconds;
	};

}
