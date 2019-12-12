//
// 2019-01-08, jjuiddong
//
// 프로토콜 생성기
// *.prt 파일을 분석해서
// protocol_dispatcher, protocol_handler, protocol_data 소스파일을 생성한다.
//
// boost library build option
//	- _HAS_ITERATOR_DEBUGGING = 0
//	- _ITERATOR_DEBUG_LEVEL = 0
//
// argument
//	-s = *.prt protocol filename
//		ex) basic.prt
//
//	-p = precompiled header filename
//		ex) stdafx.h or pch.h
//
//	-m = marshalling namespace name
//		ex) marshalling_bin or marshalling_ascii
//
// argument sample
//	-s filename -p stdafx.h -m marshalling
//

#include "pch.h"
#include <boost/program_options.hpp>

using namespace boost;
using namespace boost::program_options;
using namespace network2;


int main(int argc, char* argv[])
{
	options_description desc("Allowed options");
	desc.add_options()
		("src,s", boost::program_options::value<std::string>(), "protocol file path")
		("pch,p", boost::program_options::value<std::string>(), "precompiled header file path")
		("mar,m", boost::program_options::value<std::string>(), "marshalling name")
		;

	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);

	string fileName;
	if (vm.count("src"))
	{
		fileName = vm["src"].as<std::string>();
	}

	string pchFileName;
	if (vm.count("pch"))
	{
		pchFileName = vm["pch"].as<std::string>();
	}

	string marshalling = "marshalling";
	if (vm.count("mar"))
	{
		marshalling = vm["mar"].as<std::string>();
	}

	if (!fileName.empty())
	{
		network2::cProtocolParser parser;
		sRmi *rmiList = parser.Parse(fileName.c_str());
		if (rmiList)
		{
			compiler::WriteProtocolCode(fileName, rmiList, pchFileName, marshalling);
		}
	}

	return 0;
}
