
#include "pch.h"
#include "GenerateCode4.h"
#include <direct.h>

namespace compiler4
{
	string g_srcFolderName = "Src";
}

using namespace network2;
using namespace std;


// write default library
// - TypeVariant
// - Packet
// - SendPacket ,sendPacketBinary
// - onReceive (handler common interface)
// outputFileName: output filename => change internal, Src/~~
// return: changed outputfilename
string compiler4::WriteProtocolCode1(const string &outputFileName)
{
	const string name = common::GetFileName(outputFileName);
	const string path = common::GetFilePathExceptFileName(outputFileName);
	const string folder = (path.empty()) ? g_srcFolderName : path + "\\" + g_srcFolderName;
	_mkdir(folder.c_str());
	const string initFileName = path + "\\init0.js";
	const string changedFileName = folder + "\\" + name;

	ofstream fs;
	fs.open(changedFileName.c_str());
	if (!fs.is_open())
		return "";

	// read 'init0.js' file and then paste
	string out;
	common::cFile file;
	if (!file.ReadFile2String(initFileName, out))
		return "";

	fs << out << endl;
	return changedFileName;
}


// write worker thread communicate code
// dirPath: directory path where init1/2.js file
// outputFileName: merge output filename
bool compiler4::WriteProtocolCode2(const string &dirPath, const string &outputFileName
	, const vector<string> &dispatchers
	, const vector<string> &handlers
)
{
	const string initFileName1 = dirPath + "\\init1.js";
	const string initFileName2 = dirPath + "\\init2.js";

	ofstream fs;
	fs.open(outputFileName.c_str(), std::ios::app);
	if (!fs.is_open())
		return false;

	// read 'init1.js' file and then paste
	string out;
	common::cFile file;
	if (!file.ReadFile2String(initFileName1, out))
		return false;
	fs << out << endl;

	const string tab = "\t\t";
	for (auto &dispatcher : dispatchers)
		fs << tab << "g_dispatchers.push(new " << dispatcher << "());\n";		

	fs << "\n";

	int cnt = 0;
	for (auto &handler : handlers)
	{
		fs << tab << "var handler" << cnt << " = new " << handler << "();\n";
		fs << tab << "handler" << cnt << ".Receive = onReceive;\n";
		fs << tab << "g_handlers.push(handler" << cnt << ");\n";
		fs << "\n";
		++cnt;
	}

	// read 'init2.js' file and then paste
	out.clear();
	if (!file.ReadFile2String(initFileName2, out))
		return false;
	fs << out << endl;

	return true;
}
