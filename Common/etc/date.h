//
// Boost의 date 객체를 이용하면 날짜 계산을 쉽게 할 수 있다.
//
#pragma once


namespace common
{
	// year-month-day-hour-minutes-seconds-millseconds
	string GetCurrentDateTime();
	

	class cDateTime
	{
	public:
		cDateTime();
		cDateTime(const string &strDate);
		virtual ~cDateTime();


	public:
	};

}
