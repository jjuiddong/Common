
#include "stdafx.h"
#include "date.h"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


using namespace common;

// 2017-01-08-10-10-10
// year-month-day-hour-minutes-seconds
string common::GetCurrentDateTime()
{
	// http://stackoverflow.com/questions/22975077/how-to-convert-a-boostptime-to-string

	using namespace boost::gregorian;
	using namespace boost::posix_time;

	boost::gregorian::date dayte(boost::gregorian::day_clock::local_day());
	boost::posix_time::ptime midnight(dayte);
	boost::posix_time::ptime
		now(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration td = now - midnight;

	date::ymd_type ymd = second_clock::local_time().date().year_month_day();

	stringstream ss;
	ss << ymd.year << "-" << ymd.month.as_number() << "-" << ymd.day << "-"
		<< td.hours() << "-" << td.minutes() << "-" << td.seconds();

	return ss.str();
}
