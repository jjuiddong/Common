//
// 2018-12-30, jjuiddong
// simple data managing class
//	- read, write file
//	- data manager
//	- string table, delimeter:comma
//
//	data format
//		- data, data, data
//		- data, data, data
//		....
//
// sample data
// 1, 2, 3
// 4, 5, 6
// 7, 8, 9
//
#pragma once


namespace common
{

	class cSimpleData
	{
	public:
		cSimpleData(const StrPath &fileName = "");
		virtual ~cSimpleData();

		bool Read(const StrPath &fileName, const string &delimiter = ",");
		bool Write(const StrPath &fileName, const string &delimiter = ",");
		bool ReadLine(const StrPath &fileName, const string &delimiter = ","
			, const uint lineCount = 0);
		bool ReadFromString(const string &str, const string &delimiter = ",");
		bool IsLoad();
		void Clear();

	public:
		vector<vector<string>> m_table;
	};

}

