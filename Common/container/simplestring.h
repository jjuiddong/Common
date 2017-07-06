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
		virtual ~String() {
		}

		char* Format(const char* fmt, ...) {
			va_list args;
			va_start(args, fmt);
			vsnprintf_s(m_str, sizeof(m_str) - 1, _TRUNCATE, fmt, args);
			va_end(args);
			return m_str;
		}

		const char* GetExtend() const {
			return PathFindExtensionA(m_str);
		}


		const char* c_str() const { return (const char*)m_str; }

		operator char*() { return m_str; }
		operator const char*() const { return m_str;}

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


		String& operator = (char *str) const {
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
