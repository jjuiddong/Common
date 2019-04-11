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
		cSimpleData();
		virtual ~cSimpleData();

		bool Read(const char *fileName);
		bool Write(const char *fileName);
		void Clear();


	public:
		vector<vector<string>> m_table;
	};

}

