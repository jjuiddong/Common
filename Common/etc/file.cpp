
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


// read file all
// binary: read binary? or ascii
// dest: read data
int cFile::ReadAll(const StrPath& fileName, const bool isBinary
	, const uint size, OUT char* dest)
{
	using namespace std;

	const uint fileSize = (uint)fileName.FileSize();
	int ret = 0;
	if (isBinary)
	{
		ifstream ifs(fileName.c_str(), ios::binary);
		if (!ifs.is_open())
			return -1;
		ifs.read(dest, std::min(fileSize, size));
	}
	else
	{
		ifstream ifs(fileName.c_str());
		if (!ifs.is_open())
			return -1;
		ifs.read(dest, std::min(fileSize, size));
	}
	ret = (int)std::min(fileSize, size);
	return ret;
}


// read file all
// binary: read binary? or ascii
// return: read file byte size, pointer, fail:(0, nullptr)
std::pair<uint, char*> cFile::ReadAll2(const StrPath& fileName, const bool isBinary)
{
	using namespace std;

	const uint fileSize = (uint)fileName.FileSize();
	if (0 == fileSize)
		return { 0, nullptr };

	char* dest = new char[fileSize];
	if (isBinary)
	{
		ifstream ifs(fileName.c_str(), ios::binary);
		if (!ifs.is_open())
		{
			delete[] dest;
			return { 0, nullptr };
		}
		ifs.read(dest, fileSize);
	}
	else
	{
		ifstream ifs(fileName.c_str());
		if (!ifs.is_open())
		{
			delete[] dest;
			return { 0, nullptr };
		}
		ifs.read(dest, fileSize);
	}
	return { fileSize, dest };
}
