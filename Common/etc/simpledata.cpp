
#include "stdafx.h"
#include "simpledata.h"

using namespace common;


cSimpleData::cSimpleData(const char *fileName // = NULL
)
{
	if (fileName)
		Read(fileName);
}

cSimpleData::~cSimpleData()
{
	Clear();
}


bool cSimpleData::Read(const char *fileName)
{
	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;

	Clear();

	string line;
	while (getline(ifs, line))
	{
		vector<string> out;
		common::tokenizer2(line, ",", out);
		if (out.empty())
			continue;

		m_table.push_back(out);
	}

	return true;
}


bool cSimpleData::Write(const char *fileName)
{
	using namespace std;
	ofstream ofs(fileName);
	if (!ofs.is_open())
		return false;

	for (auto &row : m_table)
	{
		for (uint i=0; i < row.size(); ++i)
		{
			auto &col = row[i];
			ofs << col;
			if (i != row.size() - 1)
				ofs << ",";
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
