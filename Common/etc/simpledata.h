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

		template<class T> T Parse(const vector<string> &strs, const uint index
			, const T& defaultValue);


	public:
		vector<vector<string>> m_table;
	};


	// template specialization (float)
	template<>
	inline float cSimpleData::Parse(const vector<string>& strs, const uint index
		, const float &defaultValue)
	{
		if (strs.size() < (index + 1))
			return defaultValue;
		return (float)atof(strs[index].c_str());
	}

	// template specialization (Vector3)
	template<>
	inline Vector3 cSimpleData::Parse(const vector<string>& strs, const uint index
		, const Vector3& defaultValue)
	{
		if (strs.size() < (index + 3))
			return defaultValue;
		Vector3 v;
		v.x = (float)atof(strs[index + 0].c_str());
		v.y = (float)atof(strs[index + 1].c_str());
		v.z = (float)atof(strs[index + 2].c_str());
		return v;
	}

	// template specialization (Vector2)
	template<>
	inline Vector2 cSimpleData::Parse(const vector<string>& strs, const uint index
		, const Vector2& defaultValue)
	{
		if (strs.size() < (index + 2))
			return defaultValue;
		Vector2 v;
		v.x = (float)atof(strs[index + 0].c_str());
		v.y = (float)atof(strs[index + 1].c_str());
		return v;
	}

}

