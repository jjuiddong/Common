
#include "stdafx.h"
#include "simpledata.h"

using namespace common;


cSimpleData::cSimpleData(const StrPath &fileName // = ""
)
{
	if (!fileName.empty())
		Read(fileName);
}

cSimpleData::~cSimpleData()
{
	Clear();
}


bool cSimpleData::Read(const StrPath &fileName
	, const string &delimiter //= ","
)
{
	using namespace std;
	ifstream ifs(fileName.c_str());
	if (!ifs.is_open())
		return false;

	Clear();

	string line;
	while (getline(ifs, line))
	{
		vector<string> out;
		common::tokenizer2(line, delimiter, out);
		if (out.empty())
			continue;

		m_table.push_back(out);
	}

	return true;
}


bool cSimpleData::Write(const StrPath &fileName
	, const string &delimiter //= ","
)
{
	using namespace std;
	ofstream ofs(fileName.c_str());
	if (!ofs.is_open())
		return false;

	for (auto &row : m_table)
	{
		for (uint i=0; i < row.size(); ++i)
		{
			auto &col = row[i];
			ofs << col;
			if (i != row.size() - 1)
				ofs << delimiter;
		}
		ofs << endl;
	}
	return true;
}


bool cSimpleData::IsLoad()
{
	return !m_table.empty();
}


void cSimpleData::Clear()
{
	m_table.clear();
}
