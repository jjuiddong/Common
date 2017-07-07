//
// 2017-07-05, jjuiddong
// simple string
// use stack memory
//
#pragma once

#include <shlwapi.h> // file path library


namespace common
{

	template<size_t MAX>
	class String
	{
	public:
		String() {
			memset(m_str, 0, MAX);
		}
		String(const char *str) {
			strcpy_s(m_str, str);
		}
		String(const std::string &str) {
			strcpy_s(m_str, str.c_str());
		}

		virtual ~String() {
		}

		char* Format(const char* fmt, ...) {
			va_list args;
			va_start(args, fmt);
			vsnprintf_s(m_str, sizeof(m_str) - 1, _TRUNCATE, fmt, args);
			va_end(args);
			return m_str;
		}

		//------------------------------------------------------------------------------------
		// shlwapi
		bool IsRelativePath() const {
			return PathIsRelativeA(m_str) ? true : false;
		}

		bool IsFileExist() const {
			return common::IsFileExist(*this);
		}

		const char* GetFileExt() const {
			return PathFindExtensionA(m_str);
		}

		const char* GetFileName() const {
			return PathFindFileNameA(m_str);
		}

		String GetFullFileName() const {
			String str;
			if (IsRelativePath())
			{
				char curDir[MAX];
				GetCurrentDirectoryA(MAX, curDir);
				strcat_s(curDir, "/");
				strcat_s(curDir, m_str);
				GetFullPathNameA(curDir, MAX, str.m_str, NULL);
			}
			else
			{
				GetFullPathNameA(m_str, MAX, str.m_str, NULL);
			}
			return str;
		}

		String GetFilePathExceptFileName() const {
			String str = *this;
			PathRemoveFileSpecA(str.m_str);
			return str;
		}

		//------------------------------------------------------------------------------------
		// STL
		void erase(const char *str) {
			// todo:
			//str.erase(str.find("::"));
		}

		char* find(const char *str) {
			return NULL;
		}

		char* find(const String &str) {
			return NULL;
		}

		char back() {
			if (empty())
				return NULL;
			for (int i = 0; i < MAX - 1; ++i)
				if (NULL == m_str[i + 1])
					return m_str[i];
			return NULL;
		}

		bool empty() const {
			return m_str[0] == NULL;
		}

		String& lowerCase() {
			std::transform(m_str, &m_str[MAX], m_str, tolower);
			return *this;
		}

		String& upperrCase() {
			std::transform(m_str, &m_str[MAX], m_str, toupper);
			return *this;
		}

		hashcode GetHashCode() const {
			boost::hash<std::string> string_hash;
			return string_hash(m_str);
		}

		void clear() {
			memset(m_str, 0, MAX);
		}
		//------------------------------------------------------------------------------------


		const char* c_str() const { return (const char*)m_str; }

		bool operator == (const String &rhs) const {
			return !strcmp(m_str, rhs.m_str);
		}

		bool operator == (const std::string &rhs) const {
			return rhs == m_str;
		}

		bool operator == (const char *str) const {
			return !strcmp(m_str, str);
		}

		bool operator != (const String &rhs) const {
			return !(operator==(rhs));
		}

		bool operator != (const std::string &rhs) const {
			return !(operator==(rhs));
		}

		bool operator != (const char *str) const {
			return !(operator==(str));
		}


		String& operator = (const char *str) {
			strcpy_s(m_str, str);
			return *this;
		}

		String& operator = (const String &rhs) {
			if (this != &rhs) {
				strcpy_s(m_str, rhs.m_str);
			}
			return *this;
		}

		String& operator = (const std::string &rhs) {
			strcpy_s(m_str, rhs.c_str());
			return *this;
		}

		String operator + (const char *str) const {
			String v;
			v += *this;
			v += str;
			return v;
		}

		String operator + (const String &str) const {
			String v;
			v += *this;
			v += str;
			return v;
		}

		String& operator += (const char *str) {
			strcat_s(m_str, str);
			return *this;
		}


		String& operator += (const String &str) {
			strcat_s(m_str, str.m_str);
			return *this;
		}

		bool operator < (const String &rhs) const {
			return std::strcmp(m_str, rhs.m_str) < 0;
			//return cmp_str(m_str, rhs.m_str);
		}

		//bool operator > (const String &rhs) {
		//	//return std::strcmp(m_str, rhs.m_str) < 0;
		//	return cmp_str(m_str, rhs.m_str);
		//}


	public:
		char m_str[MAX];
	};


	// Define Type
	typedef String<16> Str16;
	typedef String<32> Str32;
	typedef String<64> Str64;
	typedef String<128> Str128;
	typedef String<256> StrPath;
}
