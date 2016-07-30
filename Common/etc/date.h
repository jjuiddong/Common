//
// Boost의 date 객체를 이용하면 날짜 계산을 쉽게 할 수 있다.
//
#pragma once


//#include "boost/date_time/gregorian/gregorian.hpp"
//#include "boost/date_time/posix_time/posix_time.hpp"


namespace common
{
// 	using namespace boost::gregorian;
//		using namespace boost::posix_time;
// 	std::string s("2001-10-9"); //2001-October-09
// 	date d(from_simple_string(s));
// 	std::cout << to_simple_string(d) << std::endl;
// 
// 	std::string ud("20011009"); //2001-Oct-09
// 	date d1(from_undelimited_string(ud));
// 	std::cout << to_iso_extended_string(d1) << std::endl;
// 
// 	date::ymd_type ymd = d1.year_month_day();
// 	greg_weekday wd = d1.day_of_week();
// 	std::cout << wd.as_long_string() << " "
// 		<< ymd.month.as_long_string() << " "
// 		<< ymd.day << ", " << ymd.year
// 		<< std::endl;
// 
// 	std::string s2("2001-10-10");
// 	date d2(from_simple_string(s2));
// 	date::ymd_type ymd2 = d2.year_month_day();
// 	std::cout << to_iso_extended_string(d2) << std::endl;
// 
// 	date_duration dur = d - d2;
// 	std::cout << dur.days() << std::endl;
// 
// 	if (d2 > d)
// 	{
// 		std::cout << "ok" << std::endl;
// 	}


	// 현재 시간
	//date d2 = second_clock::local_time().date();

}

