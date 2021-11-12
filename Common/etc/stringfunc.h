//
// 2019-02-23, jjuiddong
// string utility functions
//
// 2019-11-04
//	- update variant function
//		- str2variant
//		- copyvariant
//		- clearvariant
//
#pragma once


namespace common
{
	void replaceAll(string& str, const string& from, const string& to);
	string& trim(string &str);
	string trim2(const string &str);
	void trimw(wstring &str);
	wstring trimw2(const wstring &str);

	string& lowerCase(string &str);
	string lowerCase2(const string &str);
	string& upperCase(string &str);
	string upperCase2(const string &str);
	wstring& lowerCasew(wstring &str);
	wstring& upperCasew(wstring &str);

	string wstr2str(const wstring &wstr);
	wstring str2wstr(const string &str);
	string wstr2utf8(const wstring &wstr);
	string variant2str(const _variant_t &var, const bool isStringDoubleQuoto=false);
	_variant_t str2variant(const VARTYPE &vt, const string &value);
	_variant_t copyvariant(const _variant_t &var);
	void clearvariant(INOUT _variant_t &var);

	string format(const char* fmt, ...);
	wstring formatw(const char* fmt, ...);

	template<size_t MAX>
	String<char, MAX> format(const char* fmt, ...) {
		String<char, MAX> textString;
		va_list args;
		va_start(args, fmt);
		vsnprintf_s(textString.m_str, sizeof(textString.m_str), _TRUNCATE, fmt, args);
		va_end(args);
		return textString;
	}

	void tokenizer(const string &str, const string &delimeter, const string &ignoreStr, OUT vector<string> &out);
	void wtokenizer(const wstring &str, const wstring &delimeter, const wstring &ignoreStr, OUT vector<wstring> &out);
	void tokenizer2(const string &str, const string &delimeters, OUT vector<string> &out);
	void tokenizer3(const char *data, const int size, const char delimeter, OUT vector<int> &out);
	void tokenizer_space(const string &str, OUT vector<string> &out);
	void tokenizer_space(const char *str, OUT vector<string> &out);

	int strcomposite(char *buff, const int maxSize, const char delimter, const vector<int> &datas);

	int scanner(const wchar_t *buf, const wchar_t *fmt, ...);
}

