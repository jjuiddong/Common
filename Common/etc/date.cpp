
#include "stdafx.h"
#include "date.h"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <time.h>



using namespace common;


// return time string, yyyy-mm-dd-hh-mm-ss-mmm
// ex) 2017-01-08-11-05-30-010
// http://stackoverflow.com/questions/22975077/how-to-convert-a-boostptime-to-string
string common::GetCurrentDateTime()
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	boost::gregorian::date dayte(boost::gregorian::day_clock::local_day());
	boost::posix_time::ptime midnight(dayte);
	boost::posix_time::ptime
		now(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration td = now - midnight;

	date::ymd_type ymd = second_clock::local_time().date().year_month_day();

	stringstream ss;
	try {
		ss << ymd.year 
			<< "-" << std::setfill('0') << std::setw(2) << ymd.month.as_number() 
			<< "-" << std::setfill('0') << std::setw(2) << ymd.day
			<< "-" << std::setfill('0') << std::setw(2) << td.hours() 
			<< "-" << std::setfill('0') << std::setw(2) << td.minutes() 
			<< "-" << std::setfill('0') << std::setw(2) << td.seconds() 
			<< "-" << std::setfill('0') << std::setw(3) << td.total_milliseconds()%1000;
	}
	catch (...)
	{
		// nothing~
	}

	return ss.str();
}


// return time string, yyyy-mm-dd-hh-mm-ss
// ex) 2017-01-08-11-05-30
// http://stackoverflow.com/questions/22975077/how-to-convert-a-boostptime-to-string
string common::GetCurrentDateTime1()
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	boost::gregorian::date dayte(boost::gregorian::day_clock::local_day());
	boost::posix_time::ptime midnight(dayte);
	boost::posix_time::ptime
		now(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration td = now - midnight;

	date::ymd_type ymd = second_clock::local_time().date().year_month_day();

	stringstream ss;
	try {
		ss << ymd.year
			<< "-" << std::setfill('0') << std::setw(2) << ymd.month.as_number()
			<< "-" << std::setfill('0') << std::setw(2) << ymd.day
			<< "-" << std::setfill('0') << std::setw(2) << td.hours()
			<< "-" << std::setfill('0') << std::setw(2) << td.minutes()
			<< "-" << std::setfill('0') << std::setw(2) << td.seconds()
			;
	}
	catch (...)
	{
		// nothing~
	}
	return ss.str();
}


// return time string, yyyymmddhhmmssmmm
// ex) 20170108110530010
// http://stackoverflow.com/questions/22975077/how-to-convert-a-boostptime-to-string
string common::GetCurrentDateTime2()
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	boost::gregorian::date dayte(boost::gregorian::day_clock::local_day());
	boost::posix_time::ptime midnight(dayte);
	boost::posix_time::ptime
		now(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration td = now - midnight;

	date::ymd_type ymd = second_clock::local_time().date().year_month_day();

	stringstream ss;
	try {
		ss << ymd.year
			<< std::setfill('0') << std::setw(2) << ymd.month.as_number()
			<< std::setfill('0') << std::setw(2) << ymd.day
			<< std::setfill('0') << std::setw(2) << td.hours()
			<< std::setfill('0') << std::setw(2) << td.minutes()
			<< std::setfill('0') << std::setw(2) << td.seconds()
			<< std::setfill('0') << std::setw(3) << td.total_milliseconds() % 1000;
	}
	catch (...)
	{
		// nothing~
	}

	return ss.str();
}


// return uint64 time value
// yyyymmddhhmmssmmm
// ex) 20170108110530010
// http://stackoverflow.com/questions/22975077/how-to-convert-a-boostptime-to-string
unsigned __int64 common::GetCurrentDateTime3()
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	boost::gregorian::date dayte(boost::gregorian::day_clock::local_day());
	boost::posix_time::ptime midnight(dayte);
	boost::posix_time::ptime
		now(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration td = now - midnight;

	date::ymd_type ymd = second_clock::local_time().date().year_month_day();

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



// return time string, yyyymmdd
// ex) 20170108
// http://stackoverflow.com/questions/22975077/how-to-convert-a-boostptime-to-string
string common::GetCurrentDateTime4()
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	boost::gregorian::date dayte(boost::gregorian::day_clock::local_day());
	boost::posix_time::ptime midnight(dayte);
	boost::posix_time::ptime
		now(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration td = now - midnight;

	date::ymd_type ymd = second_clock::local_time().date().year_month_day();

	stringstream ss;
	try {
		ss << ymd.year
			<< std::setfill('0') << std::setw(2) << ymd.month.as_number()
			<< std::setfill('0') << std::setw(2) << ymd.day
			;
	}
	catch (...)
	{
		// nothing~
	}

	return ss.str();
}


// return time string, yyyy-mm-dd hh:mm:ss
// ex) 2017-01-08 11:05:30
string common::GetCurrentDateTime5()
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
	try {
		ss << ymd.year
			<< "-" << std::setfill('0') << std::setw(2) << ymd.month.as_number()
			<< "-" << std::setfill('0') << std::setw(2) << ymd.day
			<< " " << std::setfill('0') << std::setw(2) << td.hours()
			<< ":" << std::setfill('0') << std::setw(2) << td.minutes()
			<< ":" << std::setfill('0') << std::setw(2) << td.seconds()
			;
	}
	catch (...)
	{
		// nothing~
	}

	return ss.str();
}


// convert time string to uint64
// yyyymmddhhmmssmmm
// ex) 2017-01-08-11-05-30-010 -> 20170108110530010
uint64 common::GetCurrentDateTime6(const string &dateTime)
{
	vector<string> out;
	tokenizer(dateTime, "-", "", out);
	if (out.size() < 7)
		return 0;

	const uint64 ret = (uint64)atoi(out[0].c_str()) * 10000000000000
		+ (uint64)atoi(out[1].c_str()) * 100000000000
		+ (uint64)atoi(out[2].c_str()) * 1000000000
		+ (uint64)atoi(out[3].c_str()) * 10000000
		+ (uint64)atoi(out[4].c_str()) * 100000
		+ (uint64)atoi(out[5].c_str()) * 1000
		+ (uint64)atoi(out[6].c_str()) % 1000;

	return ret;
}


// return month day, year ex) Mon, 29 Nov 2010 21:54:29
// email date string format
string common::GetCurrentDateTime7()
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	boost::gregorian::date dayte(boost::gregorian::day_clock::local_day());
	boost::posix_time::ptime midnight(dayte);
	boost::posix_time::ptime
		now(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration td = now - midnight;

	date::ymd_type ymd = second_clock::local_time().date().year_month_day();
	date::day_of_week_type wday = second_clock::local_time().date().day_of_week();

	stringstream ss;
	try {
		ss << wday
			<< ", " << ymd.day
			<< " " << ymd.month
			<< " " << ymd.year
			<< " " << td.hours()
			<< ":" << td.minutes()
			<< ":" << td.seconds()
			;
	}
	catch (...)
	{
		// nothing~
	}
	return ss.str();
}


// return time string, yyyymmdd hh:mm
// ex) 20170108 11:05
string common::GetCurrentDateTime8()
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
	try {
		ss << ymd.year
			<< std::setfill('0') << std::setw(2) << ymd.month.as_number()
			<< std::setfill('0') << std::setw(2) << ymd.day
			<< " " << std::setfill('0') << std::setw(2) << td.hours()
			<< ":" << std::setfill('0') << std::setw(2) << td.minutes()
			;
	}
	catch (...)
	{
		// nothing~
	}
	return ss.str();
}


// return : duration day, yyyymmdd1 - yyyymmdd0
// ex) 20200120 - 20191201
int common::DateCompare(const int ymd1, const int ymd0)
{
	// nothing~
	return 0;
}


// return : duration day, ymd1 - ymd0
// ex) yyyy0 : 2019
int common::DateCompare2(const int yyyy1, const int mm1, const int dd1
	, const int yyyy0, const int mm0, const int dd0)
{
	using namespace std;
	using namespace std::chrono;

	std::tm d0;
	ZeroMemory(&d0, sizeof(d0));
	d0.tm_year = yyyy0 - 1900;
	d0.tm_mon = mm0 - 1;
	d0.tm_mday = dd0;

	std::tm d1;
	ZeroMemory(&d1, sizeof(d1));
	d1.tm_year = yyyy1 - 1900;
	d1.tm_mon = mm1 - 1;
	d1.tm_mday = dd1;

	typedef duration<int, ratio_multiply<hours::period, ratio<24>>::type> days;
	auto t0 = std::chrono::system_clock::from_time_t(std::mktime(&d0));
	auto t1 = std::chrono::system_clock::from_time_t(std::mktime(&d1));
	auto diff_in_days = std::chrono::duration_cast<days>(t1 - t0);
	return (int)diff_in_days.count();
}


//--------------------------------------------------------------------------------------
cDateTime::cDateTime()
{
}

cDateTime::cDateTime(const string &strDate)
{
}

cDateTime::~cDateTime()
{
}


bool cDateTime::Parse(const char *str)
{
	return true;
}
