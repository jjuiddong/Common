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
			const size_t len = min(strlen(str), MAX - 1);
			strncpy_s(m_str, str, len);
			m_str[len] = NULL;
		}
		String(const std::string &str) {
			const size_t len = min(str.size(), MAX - 1);
			strncpy_s(m_str, str.c_str(), len);
			m_str[len] = NULL;
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

		String convertToString(double num) const {
			String str;
			sprintf_s(str.m_str, "%.1f", num);
			return str;
		}

		double roundOff(double n) const {
			double d = n * 100.0f;
			int i = (int)(d + 0.5);
			d = (float)i / 100.0f;
			return d;
		}

		String convertSize(size_t size) const {
			static const char *SIZES[] = { "B", "KB", "MB", "GB" };
			int div = 0;
			size_t rem = 0;

			while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES)) {
				rem = (size % 1024);
				div++;
				size /= 1024;
			}

			double size_d = (float)size + (float)rem / 1024.0;
			String result = convertToString(roundOff(size_d)) + " " + SIZES[div];
			return result;
		}

		__int64 FileSize() const {
			struct __stat64 buf;
			if (_stat64(m_str, &buf) != 0)
				return -1; // error, could use errno to find out more
			return buf.st_size;
		}

		String FileSizeStr() const {
			return convertSize((size_t)FileSize());
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

		// only return filname except extends
		String GetFileNameExceptExt() const {
			String str = PathFindFileNameA(m_str);
			PathRemoveExtensionA(str.m_str);
			return str;
		}

		// return full filname except extends
		String GetFileNameExceptExt2() const {
			String str = *this;
			char *name = PathFindFileNameA(str.m_str);
			PathRemoveExtensionA(name);
			return str;
		}


		//------------------------------------------------------------------------------------
		// STL
		void erase(const char *str) {
			char *p = (char*)find(str);
			if (!p || !*p)
				return;

			std::rotate(p, p + strlen(str), m_str + strlen(m_str));
			const int len = strlen(m_str) - strlen(str);
			m_str[len] = NULL;
		}

		const char* find(const char *str) const {
			const char *p = m_str;
			while (*p)
			{
				const char *n = p;
				const char *c = str;
				while (*n && *c)
				{
					if (*n != *c)
						break;
					++n;
					++c;
				}

				if (!*c) // found
					return p;

				++p; // not found
			}
			return NULL;
		}

		const char* find(const String &str) const {
			return find(str.m_str);
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

		size_t size() const {
			return (size_t)strlen(m_str);
		}

		void clear() {
			memset(m_str, 0, MAX);
		}
		//------------------------------------------------------------------------------------

		//------------------------------------------------------------------------------------
		// ETC
		std::wstring wstr() const
		{
			int len;
			int slength = (int)size() + 1;
			len = ::MultiByteToWideChar(CP_ACP, 0, m_str, slength, 0, 0);
			wchar_t* buf = new wchar_t[len];
			::MultiByteToWideChar(CP_ACP, 0, m_str, slength, buf, len);
			std::wstring r(buf);
			delete[] buf;
			return r;
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
			const size_t len = min(strlen(str), MAX - 1);
			strncpy_s(m_str, str, len);
			m_str[len] = NULL;
			return *this;
		}

		String& operator = (const String &rhs) {
			if (this != &rhs) {
				operator = (rhs.m_str);
			}
			return *this;
		}

		String& operator = (const std::string &rhs) {
			return operator = (rhs.c_str());
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
			const size_t len1 = strlen(m_str);
			if (len1 >= (MAX - 1))
				return *this;

			const size_t len2 = strlen(str);
			const size_t cpLen = min(len2, MAX-len1-1);
			strncat_s(m_str, str, cpLen);
			m_str[len1 + cpLen] = NULL;		
			return *this;
		}


		String& operator += (const String &str) {
			if (this == &str)
				return *this;

			const size_t len1 = strlen(m_str);
			if (len1 >= (MAX - 1))
				return *this;

			const size_t len2 = strlen(str.m_str);
			const size_t cpLen = min(len2, MAX - len1 - 1);
			strncat_s(m_str, str.m_str, cpLen);
			m_str[len1 + cpLen] = NULL;
			return *this;
		}

		bool operator < (const String &rhs) const {
			return std::strcmp(m_str, rhs.m_str) < 0;
		}


	public:
		char m_str[MAX];
	};


	// Define Type
	typedef String<16> Str16;
	typedef String<32> Str32;
	typedef String<64> Str64;
	typedef String<64> StrId;
	typedef String<128> Str128;
	typedef String<128> StrPath;
	typedef String<256> StrGlobalPath;
}
