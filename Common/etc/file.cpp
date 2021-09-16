
#include "stdafx.h"
#include "file.h"


using namespace common;

cFile::cFile()
{
}

cFile::~cFile()
{
}


// read file and convert to string
bool cFile::ReadFile2String(const StrPath &fileName
	, OUT string &out)
{
	using namespace std;

	string str;
	ifstream ifs(fileName.c_str());
	if (!ifs.is_open())
		return false;

	string line;
	while (getline(ifs, line))
	{
		out += line;
		out += "\n";
	}

	return true;
}
