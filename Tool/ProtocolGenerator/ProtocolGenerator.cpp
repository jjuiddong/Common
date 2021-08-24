//
// 2019-01-08, jjuiddong
//
// Protocol Generator
// parse <*.prt> protocol script file
// generate protocol_dispatcher, protocol_handler, protocol_data source file
//
// argument
//	-s = *.prt protocol filename
//		ex) basic.prt
//
//	-p = precompiled header filename
//		ex) stdafx.h or pch.h
//
//	ignore marsalling argument
//		-m = marshalling namespace name
//			ex) marshalling_bin or marshalling_ascii or marshalling_json
//
// argument sample
//	-s filename -p stdafx.h -m marshalling
//
// 2020-11-10
//	- add json marsalling format
//	- remove marshalling argument
//	- add protocol format command
//		ex) protocol c2s 1000  <-- binary format default
//		ex) protocol c2s 1000 binary
//		ex) protocol c2s 1000 ascii
//		ex) protocol c2s 1000 json
//
// 2021-08-22
//	- generate typescript code
//	- generate javascript code
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
	try {
		store(parse_command_line(argc, argv, desc), vm);
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}

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
		//marshalling = vm["mar"].as<std::string>();
		std::cout << "-m, -mar argument ignored!" << std::endl;
	}

	if (!fileName.empty())
	{
		network2::cProtocolParser parser;
		sStmt *stmt = parser.Parse(fileName);
		if (stmt && stmt->protocol)
			compiler::WriteProtocolCode(fileName, stmt->protocol, pchFileName);
		if (stmt && stmt->protocol)
			compiler2::WriteProtocolCode(fileName, stmt->protocol, stmt->type);
		if (stmt && stmt->protocol)
			compiler3::WriteProtocolCode(fileName, stmt->protocol, stmt->type);
	}

	return 0;
}
