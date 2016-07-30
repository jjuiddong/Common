//
// 환경파일 읽고, 저장하는 기능.
//
#pragma once

namespace common
{

	class cConfig
	{
	public:
		cConfig();
		virtual ~cConfig();

		bool Read(const string &fileName);
		bool Write(const string &fileName);
		void ParseStr(const string &str);

		bool GetBool(const string &key, const bool defaultValue=false);
		float GetFloat(const string &key, const float defaultValue = 0);
		double GetDouble(const string &key, const double defaultValue = 0);
		int GetInt(const string &key, const int defaultValue = 0);
		string GetString(const string &key, const string &defaultValue = "");


	protected:
		bool Parse(const string &fileName);
		virtual void InitDefault() {}
		virtual void UpdateParseData() {}


	public:
		map<string, string> m_options;
	};

}
