
#include "stdafx.h"
#include "date2.h"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace common;


//-------------------------------------------------
// cDateTime2
//-------------------------------------------------
cDateTime2::cDateTime2()
	: m_t(0)
{
}

// dateTime: yyyymmddhhmmssmmm
cDateTime2::cDateTime2(const uint64 dateTime)
	: m_t(0)
{
	SetTime(dateTime);
}

// dateTime: YYYY-MM-DD-HH-MM-SS-mmm
cDateTime2::cDateTime2(const string &dateTime)
	: m_t(0)
{
	SetTime(dateTime);
}

cDateTime2::~cDateTime2()
{
}


// set date:
//	datetime: format yyyymmddhhmmssmmm
//			ex) 20170108110530010
//
void cDateTime2::SetTime(const uint64 dateTime)
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	const uint64 year = dateTime / 10000000000000;
	const uint64 month = (dateTime / 100000000000) % 100;
	const uint64 day = (dateTime / 1000000000) % 100;
	const uint64 hour = (dateTime / 10000000) % 100;
	const uint64 minute = (dateTime / 100000) % 100;
	const uint64 second = (dateTime / 1000) % 100;
	const uint64 millis = (dateTime) % 1000;

	m_t = 0;
	try 
	{
		const ptime t = ptime(date((uint)year, (uint)month, (uint)day)
					, time_duration((uint)hour, (uint)minute, (uint)second));
		const ptime time_t_epoch(date(1970, 1, 1));
		const time_duration diff = t - time_t_epoch;
		m_t = diff.total_milliseconds();
		m_t += millis;
	}
	catch (...)
	{
		// nothing
	}
}


// set time
// dateTime: YYYY-MM-DD-HH-MM-SS-mmm
void cDateTime2::SetTime(const string &dateTime)
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	if (dateTime.empty())
		return;

	uint64 year = 0;
	uint64 month = 0;
	uint64 day = 0;
	uint64 hour = 0;
	uint64 minute = 0;
	uint64 second = 0;
	uint64 millis = 0;

	int state = 0;
	int val = 0;
	const char* c = dateTime.c_str();
	while (*c != '\0')
	{
		if ('-' == *c)
		{
			switch (state)
			{
			case 0: year = val; break;
			case 1: month = val; break;
			case 2: day = val; break;
			case 3: hour = val; break;
			case 4: minute = val; break;
			case 5: second = val; break;
			case 6: millis = val; break;
			}
			val = 0;
			++state;
		}
		else
		{
			val = (val * 10) + *c - '0';
		}
		++c;
	}

	m_t = 0;
	try
	{
		const ptime t = ptime(date((uint)year, (uint)month, (uint)day)
			, time_duration((uint)hour, (uint)minute, (uint)second));
		const ptime time_t_epoch(date(1970, 1, 1));
		const time_duration diff = t - time_t_epoch;
		m_t = diff.total_milliseconds();
		m_t += millis;
	}
	catch (...)
	{
		// nothing
	}
}


void cDateTime2::SetTime(const cDateTime2 &dateTime)
{
	m_t = dateTime.m_t;
}


void cDateTime2::UpdateCurrentTime()
{
	*this = cDateTime2::Now();
}


// return time value uint64
//	yyyymmddhhmmssmmm
//	ex) 20170108110530010
uint64 cDateTime2::GetTimeInt64() const
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	const ptime time_t_epoch(date(1970, 1, 1));
	const ptime t = time_t_epoch + milliseconds(m_t);

	date::ymd_type ymd = t.date().year_month_day();
	time_duration td = t.time_of_day();

	unsigned __int64 ret = 0;
	try {
		ret = (unsigned __int64)ymd.year * 10000000000000
			+ (unsigned __int64)ymd.month.as_number() * 100000000000
			+ (unsigned __int64)ymd.day * 1000000000
			+ (unsigned __int64)td.hours() * 10000000
			+ (unsigned __int64)td.minutes() * 100000
			+ (unsigned __int64)td.seconds() * 1000
			+ (unsigned __int64)td.total_milliseconds() % 1000;
	}
	catch (...)
	{
		// nothing~
	}
	return ret;
}


// return time string, yyyy-mm-dd hh:mm:ss:mmm
Str32 cDateTime2::GetTimeStr() const {
	return GetTimeStr(GetTimeInt64());
}

// return time string, yyyy-mm-dd-hh-mm-ss-mmm
Str32 cDateTime2::GetTimeStr2() const {
	return GetTimeStr2(GetTimeInt64());
}

// return time string, yyyy-mm-dd hh:mm:ss (ignore milliseconds)
Str32 cDateTime2::GetTimeStr3() const {
	return GetTimeStr3(GetTimeInt64());
}

// return time string, yyyy-mm-dd hh-mm-ss (ignore milliseconds)
Str32 cDateTime2::GetTimeStr3_1() const {
	return GetTimeStr3_1(GetTimeInt64());
}

// return time string, yyyy-mm-dd
Str32 cDateTime2::GetTimeStr4() const {
	return GetTimeStr4(GetTimeInt64());
}

// return time string, yyyymmdd
Str32 cDateTime2::GetTimeStr5() const {
	return GetTimeStr5(GetTimeInt64());
}


// add milliseconds
cDateTime2& cDateTime2::operator+=(const uint64 &milliseconds)
{
	m_t += milliseconds;
	return *this;
}
cDateTime2& cDateTime2::operator-=(const uint64 &milliseconds)
{
	m_t -= milliseconds;
	return *this;
}

bool cDateTime2::operator<(const cDateTime2 &rhs) const
{
	return m_t < rhs.m_t;
}
bool cDateTime2::operator<=(const cDateTime2 &rhs) const
{
	return m_t <= rhs.m_t;
}
bool cDateTime2::operator>(const cDateTime2 &rhs) const
{
	return m_t > rhs.m_t;
}
bool cDateTime2::operator>=(const cDateTime2 &rhs) const
{
	return m_t >= rhs.m_t;
}
bool cDateTime2::operator==(const cDateTime2 &rhs) const
{
	return m_t == rhs.m_t;
}
cDateTime2 cDateTime2::operator-(const cDateTime2 &rhs) const
{
	cDateTime2 tmp;
	tmp.m_t = m_t - rhs.m_t;
	return tmp;
}
cDateTime2 cDateTime2::operator+(const cDateTime2 &rhs) const
{
	cDateTime2 tmp;
	tmp.m_t = m_t + rhs.m_t;
	return tmp;
}



// return time string
// 20170108110530010 -> 2017-01-08 11:05:30:010
Str32 cDateTime2::GetTimeStr(const uint64 dateTime)
{
	const uint64 year = dateTime / 10000000000000;
	const uint64 month = (dateTime / 100000000000) % 100;
	const uint64 day = (dateTime / 1000000000) % 100;
	const uint64 hour = (dateTime / 10000000) % 100;
	const uint64 minute = (dateTime / 100000) % 100;
	const uint64 second = (dateTime / 1000) % 100;
	const uint64 millis = (dateTime) % 1000;

	Str32 ret;
	ret.Format("%d-%02d-%02d %02d:%02d:%02d:%03d"
		, (uint)year, (uint)month, (uint)day
		, (uint)hour, (uint)minute, (uint)second, (uint)millis);
	return ret;
}


// return time string
// 20170108110530010 -> 2017-01-08-11-05-30-010
Str32 cDateTime2::GetTimeStr2(const uint64 dateTime)
{
	const uint64 year = dateTime / 10000000000000;
	const uint64 month = (dateTime / 100000000000) % 100;
	const uint64 day = (dateTime / 1000000000) % 100;
	const uint64 hour = (dateTime / 10000000) % 100;
	const uint64 minute = (dateTime / 100000) % 100;
	const uint64 second = (dateTime / 1000) % 100;
	const uint64 millis = (dateTime) % 1000;

	Str32 ret;
	ret.Format("%d-%02d-%02d-%02d-%02d-%02d-%03d"
		, (uint)year, (uint)month, (uint)day
		, (uint)hour, (uint)minute, (uint)second, (uint)millis);
	return ret;
}


// return time string (ignore milliseconds)
// 20170108110530010 -> 2017-01-08 11:05:30
Str32 cDateTime2::GetTimeStr3(const uint64 dateTime)
{
	const uint64 year = dateTime / 10000000000000;
	const uint64 month = (dateTime / 100000000000) % 100;
	const uint64 day = (dateTime / 1000000000) % 100;
	const uint64 hour = (dateTime / 10000000) % 100;
	const uint64 minute = (dateTime / 100000) % 100;
	const uint64 second = (dateTime / 1000) % 100;
	//const uint64 millis = (dateTime) % 1000;

	Str32 ret;
	ret.Format("%d-%02d-%02d %02d:%02d:%02d"
		, (uint)year, (uint)month, (uint)day
		, (uint)hour, (uint)minute, (uint)second);
	return ret;
}


// return time string (ignore milliseconds)
// 20170108110530010 -> 2017-01-08 11-05-30
Str32 cDateTime2::GetTimeStr3_1(const uint64 dateTime)
{
	const uint64 year = dateTime / 10000000000000;
	const uint64 month = (dateTime / 100000000000) % 100;
	const uint64 day = (dateTime / 1000000000) % 100;
	const uint64 hour = (dateTime / 10000000) % 100;
	const uint64 minute = (dateTime / 100000) % 100;
	const uint64 second = (dateTime / 1000) % 100;
	//const uint64 millis = (dateTime) % 1000;

	Str32 ret;
	ret.Format("%d-%02d-%02d %02d-%02d-%02d"
		, (uint)year, (uint)month, (uint)day
		, (uint)hour, (uint)minute, (uint)second);
	return ret;
}


// return time string (only year-month-date)
// 20170108110530010 -> 2017-01-08
Str32 cDateTime2::GetTimeStr4(const uint64 dateTime)
{
	const uint64 year = dateTime / 10000000000000;
	const uint64 month = (dateTime / 100000000000) % 100;
	const uint64 day = (dateTime / 1000000000) % 100;

	Str32 ret;
	ret.Format("%d-%02d-%02d"
		, (uint)year, (uint)month, (uint)day);
	return ret;
}


// return time string (only year-month-date)
// 20170108110530010 -> 20170108
Str32 cDateTime2::GetTimeStr5(const uint64 dateTime)
{
	const uint64 year = dateTime / 10000000000000;
	const uint64 month = (dateTime / 100000000000) % 100;
	const uint64 day = (dateTime / 1000000000) % 100;

	Str32 ret;
	ret.Format("%d%02d%02d", (uint)year, (uint)month, (uint)day);
	return ret;
}


void cDateTime2::GetTimeValue(const uint64 dateTime, OUT sDateTime &out)
{
	out.year = (int)(dateTime / 10000000000000);
	out.month = (int)((dateTime / 100000000000) % 100);
	out.day = (int)((dateTime / 1000000000) % 100);
	out.hour = (int)((dateTime / 10000000) % 100);
	out.min = (int)((dateTime / 100000) % 100);
	out.sec = (int)((dateTime / 1000) % 100);
	out.millis = (int)((dateTime) % 1000);
}


// return current time object
cDateTime2 cDateTime2::Now()
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	const ptime now = microsec_clock::local_time();
	const ptime time_t_epoch(date(1970, 1, 1));
	const time_duration diff = now - time_t_epoch;

	cDateTime2 tmp;
	tmp.m_t = diff.total_milliseconds();
	return tmp;
}


//-------------------------------------------------
// cDateTimeRange
//-------------------------------------------------
cDateTimeRange::cDateTimeRange()
	: m_first(0)
	, m_second(0)
{
}

cDateTimeRange::cDateTimeRange(const uint64 dateTime1, const uint64 dateTime2)
	: m_first(dateTime1)
	, m_second(dateTime2)
{
}

cDateTimeRange::cDateTimeRange(const cDateTime2 dateTime1, const cDateTime2 dateTime2)
	: m_first(dateTime1)
	, m_second(dateTime2)
{
}

cDateTimeRange::~cDateTimeRange()
{
}


void cDateTimeRange::SetTimeRange(const uint64 dateTime1, const uint64 dateTime2)
{
	m_first = dateTime1;
	m_second = dateTime2;
}


void cDateTimeRange::SetTimeRange(const cDateTime2 dateTime1, const cDateTime2 dateTime2)
{
	m_first = dateTime1;
	m_second = dateTime2;
}


// return m_first = min(m_first, rhs.m_first)
//		  m_second = max(m_second, rhs.m_second)
cDateTimeRange cDateTimeRange::Max(const cDateTimeRange &rhs)
{
	return cDateTimeRange(
		(m_first.m_t == 0) ? rhs.m_first : min(m_first, rhs.m_first)
		, max(m_second, rhs.m_second));
}


cDateTime2 cDateTimeRange::Interpol(const double alpha) const
{
	cDateTime2 dt;
	dt.m_t = m_first.m_t + (uint64)((double)GetRange() * alpha);
	return dt;
}


uint64 cDateTimeRange::GetRange() const
{
	return (m_second.m_t - m_first.m_t);
}
