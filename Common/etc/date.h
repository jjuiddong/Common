//
// 2019-03-28, jjuiddong
// date time
//
#pragma once


namespace common
{
	// year-month-day-hour-minutes-seconds-millseconds
	string GetCurrentDateTime();
	string GetCurrentDateTime2();
	unsigned __int64 GetCurrentDateTime3();
	string GetCurrentDateTime4();
	string GetCurrentDateTime5();
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
