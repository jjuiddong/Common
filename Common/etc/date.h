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
