
#include "stdafx.h"
#include "file.h"


using namespace common;

cFile::cFile()
{
}

cFile::~cFile()
{
}


// 파일 전체를 읽어서 문자열로 리턴한다.
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
