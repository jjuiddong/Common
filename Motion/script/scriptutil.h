//
// 스크립트를 분석하는데 필요한, 전역 변수와 함수들을 모아놓았다.
//
#pragma once


namespace script
{
	// symbol
	namespace FIELD_TYPE
	{
		enum TYPE {
			T_INT = 1,
			T_SHORT,
			T_UINT,
			T_FLOAT,
			T_DOUBLE,
			T_BOOL,
		};
	};

	struct sFieldData
	{
		union
		{
			bool bVal;
			short sVal;
			int iVal;
			u_int uVal;
			float fVal;
			double dVal;
			char buff[8];
		};

		FIELD_TYPE::TYPE type;

		bool operator== (const sFieldData &rhs);
		const sFieldData& operator=(const sFieldData &rhs);
	};

	extern map<string, sFieldData> g_symbols;
	extern bool g_isLog;
	void ClearSymbols();



	extern const char *g_numStr;
	extern const int g_numLen;
	extern const char *g_strStr;
	extern const int g_strLen;
	extern const char *g_strStr2;
	extern const int g_strLen2;


	bool compare(string &src, char c);
	bool check(string &src, char c);
	bool match(string &src, char c);
	string id(string &src);
	string line(string &src);
	float number(string &src);
	void bigEndian(BYTE *buffer, const int size);

}
