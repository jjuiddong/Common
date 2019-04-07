
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

cDateTime2::cDateTime2(const uint64 dateTime)
	: m_t(0)
{
	SetTime(dateTime);
}

cDateTime2::~cDateTime2()
{
}


// datetime:
//	20170108110530010
//	year-month-day-hour-minutes-seconds-millseconds
//	yyyymmddhhmmssmmm
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


void cDateTime2::SetTime(const cDateTime2 &dateTime)
{
	m_t = dateTime.m_t;
}


void cDateTime2::UpdateCurrentTime()
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	const ptime now = microsec_clock::local_time();	
	const ptime time_t_epoch(date(1970, 1, 1));
	const time_duration diff = now - time_t_epoch;
	m_t = diff.total_milliseconds();
}


// add micro seconds
cDateTime2& cDateTime2::operator+=(const uint64 &milliseconds)
{
	m_t += milliseconds;
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


// return:
//	20170108110530010
//	year-month-day-hour-minutes-seconds-millseconds
//	yyyymmddhhmmssmmm
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


// return time string
// 2017-01-08 11:05:30:010
Str32 cDateTime2::GetTimeStr() const
{
	return GetTimeStr(GetTimeInt64());
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
