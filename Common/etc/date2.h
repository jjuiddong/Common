//
// 2019-03-28, jjuiddong
// date time by milliseconds using boost::posix_time
//
// reference
//	http://m.blog.daum.net/naivewolf/1979426
//	https://stackoverflow.com/questions/6734375/get-current-time-in-milliseconds-using-c-and-boost
//
//
#pragma once

#include "boost/date_time/local_time/local_time.hpp"

namespace common
{

	struct sDateTime
	{
		int year;
		int month;
		int day;
		int hour;
		int min;
		int sec;
		int millis;
	};

	// Date Time by milliseconds
	class cDateTime2
	{
	public:
		cDateTime2();
		cDateTime2(const uint64 dateTime);
		virtual ~cDateTime2();

		void SetTime(const uint64 dateTime);
		void SetTime(const cDateTime2 &dateTime);
		void UpdateCurrentTime();
		uint64 GetTimeInt64() const;
		Str32 GetTimeStr() const; // yyyy-mm-dd hh:mm:ss:mmm
		Str32 GetTimeStr2() const; // yyyy-mm-dd-hh-mm-ss-mmm
		Str32 GetTimeStr3() const; // yyyy-mm-dd hh:mm:ss
		Str32 GetTimeStr4() const; // yyyy-mm-dd
		Str32 GetTimeStr5() const; // yyyymmdd

		static cDateTime2 Now();
		static Str32 GetTimeStr(const uint64 dateTime); // yyyy-mm-dd hh:mm:ss:mmm
		static Str32 GetTimeStr2(const uint64 dateTime); // yyyy-mm-dd-hh-mm-ss-mmm
		static Str32 GetTimeStr3(const uint64 dateTime); // yyyy-mm-dd hh:mm:ss
		static Str32 GetTimeStr4(const uint64 dateTime); // yyyy-mm-dd
		static Str32 GetTimeStr5(const uint64 dateTime); // yyyymmdd
		static void GetTimeValue(const uint64 dateTime, OUT sDateTime &out);

		cDateTime2& operator+=(const uint64 &milliSeconds);
		cDateTime2& operator-=(const uint64 &milliSeconds);
		bool operator<(const cDateTime2 &rhs) const;
		bool operator<=(const cDateTime2 &rhs) const;
		bool operator>(const cDateTime2 &rhs) const;
		bool operator>=(const cDateTime2 &rhs) const;
		bool operator==(const cDateTime2 &rhs) const;
		cDateTime2 operator-(const cDateTime2 &rhs) const;
		cDateTime2 operator+(const cDateTime2 &rhs) const;


	public:
		uint64 m_t; // milliseconds clock
	};



	// Date Time Range Class
	class cDateTimeRange
	{
	public:
		cDateTimeRange();
		cDateTimeRange(const uint64 dateTime1, const uint64 dateTime2);
		cDateTimeRange(const cDateTime2 dateTime1, const cDateTime2 dateTime2);
		virtual ~cDateTimeRange();

		void SetTimeRange(const uint64 dateTime1, const uint64 dateTime2);
		void SetTimeRange(const cDateTime2 dateTime1, const cDateTime2 dateTime2);
		cDateTimeRange Max(const cDateTimeRange &rhs);
		cDateTime2 Interpol(const double alpha) const;
		uint64 GetRange() const;


	public:
		cDateTime2 m_first; // timerange less than second
		cDateTime2 m_second; // timerange greater than first
	};

}
