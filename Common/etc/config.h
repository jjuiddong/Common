//
// 환경파일 읽고, 저장하는 기능.
//
// 2019-03-05, jjuiddong
//	- check UTF-8 with BOM
//		- https://stackoverflow.com/questions/6769311/how-windows-notepad-interpret-characters
//		- Notepad normally uses ANSI encoding, so if it reads the file as UTF - 8 
//		  then it has to guess the encoding based on the data in the file.
//		  If you save a file as UTF - 8, Notepad will put the BOM(byte order mark) 
//		  EF BB BF at the beginning of the file.
//
#pragma once

namespace common
{

	class cConfig
	{
	public:
		cConfig(const string &fileName = "");
		virtual ~cConfig();

		bool Read(const string &fileName);
		bool Write(const string &fileName);
		void ParseStr(const string &str);

		bool GetBool(const string &key, const bool defaultValue=false);
		float GetFloat(const string &key, const float defaultValue = 0);
		double GetDouble(const string &key, const double defaultValue = 0);
		int GetInt(const string &key, const int defaultValue = 0);
		string GetString(const string &key, const string &defaultValue = "");
		Vector3 GetVector3(const string &key, const Vector3 &defaultValue = Vector3(0, 0, 0));

		void SetValue(const StrId &key, const char *value);
		void SetValue(const StrId &key, const bool value);
		void SetValue(const StrId &key, const int value);
		void SetValue(const StrId &key, const float value);
		void SetValue(const StrId &key, const double value);


	protected:
		bool Parse(const string &fileName);
		bool CheckUTF8withBOM(std::ifstream &ifs);
		virtual void InitDefault() {}
		virtual void UpdateParseData() {}


	public:
		map<string, string> m_options;
	};

}
