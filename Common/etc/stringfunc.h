//
// 2019-02-23, jjuiddong
// string utility functions
//
#pragma once


namespace common
{
	void replaceAll(string& str, const string& from, const string& to);
	string& trim(string &str);
	void trimw(wstring &str);

	string& lowerCase(string &str);
	string& upperCase(string &str);
	wstring& lowerCasew(wstring &str);
	wstring& upperCasew(wstring &str);

	string wstr2str(const wstring &wstr);
	wstring str2wstr(const string &str);
	string wstr2utf8(const wstring &wstr);
	string variant2str(const _variant_t &var, const bool isStringDoubleQuoto=false);
	_variant_t str2variant(const VARTYPE &vt, const string &value);

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

	int strcomposite(char *buff, const int maxSize, const char delimter, const vector<int> &datas);

	int scanner(const wchar_t *buf, const wchar_t *fmt, ...);
}

