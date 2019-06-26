//
// 2017-07-05, jjuiddong
// simple string
// use stack memory
//
// 2017-08-24
//	- add wchar_t type
//
//
#pragma once

#include <sys/stat.h>
#include <shlwapi.h> // file path library
#pragma comment(lib, "shlwapi")


namespace common
{

	template<class CharT, size_t MAX>
	class String
	{
	};

	template<size_t MAX>
	class String<char, MAX>
	{
	public:
		String() {
			memset(m_str, 0, sizeof(char)*MAX);
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

		void trim() {
			// trim forward
			for (int i = 0; i < (int)size(); ++i)
			{
				if ((m_str[i] == '\n') || (m_str[i] == '\t') || (m_str[i] == '\r') || (m_str[i] == ' '))
				{
					const size_t len = size();
					common::rotateleft(m_str, i+1, len);
					m_str[len - 1] = NULL;

					--i;
				}
				else
					break;
			}

			// trim backward
			for (int i = (int)size() - 1; i >= 0; --i)
			{
				if ((m_str[i] == '\n') || (m_str[i] == '\t') || (m_str[i] == '\r') || (m_str[i] == ' '))
				{
					m_str[i] = NULL;
				}
				else
					break;
			}

		}

		//------------------------------------------------------------------------------------
		// shlwapi
		bool IsRelativePath() const {
			return PathIsRelativeA(m_str) ? true : false;
		}

		bool IsFileExist() const {
			return common::IsFileExist(m_str);
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

		// only return filename except extends
		String GetFileNameExceptExt() const {
			String str = PathFindFileNameA(m_str);
			PathRemoveExtensionA(str.m_str);
			return str;
		}

		// return full filename except extends
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
		//std::wstring wstr() const
		//{
		//	int len;
		//	int slength = (int)size() + 1;
		//	len = ::MultiByteToWideChar(CP_ACP, 0, m_str, slength, 0, 0);
		//	wchar_t buf[MAX];
		//	ZeroMemory(buf, sizeof(buf));
		//	len = min(MAX-1, len);
		//	::MultiByteToWideChar(CP_ACP, 0, m_str, slength, buf, len);
		//	std::wstring r(buf);
		//	return r;
		//}

		String<wchar_t, MAX> wstr() const
		{
			int len;
			int slength = (int)size() + 1;
			len = ::MultiByteToWideChar(CP_ACP, 0, m_str, slength, 0, 0);
			String<wchar_t, MAX> buf;
			len = min(MAX - 1, len);
			::MultiByteToWideChar(CP_ACP, 0, m_str, slength, buf.m_str, len);
			return buf;
		}

		String<wchar_t, MAX> wstrUTF8() const
		{
			int len;
			int slength = (int)size() + 1;
			len = ::MultiByteToWideChar(CP_UTF8, 0, m_str, slength, 0, 0);
			String<wchar_t, MAX> buf;
			len = min(MAX - 1, len);
			::MultiByteToWideChar(CP_UTF8, 0, m_str, slength, buf.m_str, len);
			return buf;
		}


		// UTF-8 -> Default Ansi
		String ansi() const
		{
			int len;
			int slength = (int)size() + 1;
			len = ::MultiByteToWideChar(CP_UTF8, 0, m_str, slength, 0, 0);
			String<wchar_t, MAX> buf;
			len = min(MAX - 1, len);
			::MultiByteToWideChar(CP_UTF8, 0, m_str, slength, buf.m_str, len);

			String buf2;
			const int len2 = ::WideCharToMultiByte(CP_ACP, 0, buf.m_str, len, 0, 0, NULL, FALSE);
			::WideCharToMultiByte(CP_ACP, 0, buf.m_str, len, buf2.m_str, len2, NULL, FALSE);
			return buf2;
		}

		// Default Ansi -> UTF-8
		String utf8() const
		{
			int len;
			int slength = (int)size() + 1;
			len = ::MultiByteToWideChar(CP_ACP, 0, m_str, slength, 0, 0);
			String<wchar_t, MAX> buf;
			len = min(MAX - 1, len);
			::MultiByteToWideChar(CP_ACP, 0, m_str, slength, buf.m_str, len);

			String buf2;
			const int len2 = ::WideCharToMultiByte(CP_UTF8, 0, buf.m_str, len, 0, 0, NULL, FALSE);
			::WideCharToMultiByte(CP_UTF8, 0, buf.m_str, len, buf2.m_str, len2, NULL, FALSE);
			return buf2;
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
			if (!str) 
				return *this;
			if (m_str != str)
			{
				const size_t len = min(strlen(str), MAX - 1);
				strncpy_s(m_str, str, len);
				m_str[len] = NULL;
			}
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

		String& operator += (const char c) {
			const size_t len1 = strlen(m_str);
			if ((len1+1) >= (MAX - 1))
				return *this;

			m_str[len1] = c;
			m_str[len1 + 1] = NULL;
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
		const size_t SIZE = MAX;
	};


	template<size_t MAX>
	class String<wchar_t, MAX>
	{
	public:
		String() {
			memset(m_str, 0, sizeof(wchar_t)*MAX);
		}
		String(const wchar_t *str) {
			const size_t len = min(wcslen(str), MAX - 1);
			wcsncpy_s(m_str, str, len);
			m_str[len] = NULL;
		}
		String(const std::wstring &str) {
			const size_t len = min(str.size(), MAX - 1);
			wcsncpy_s(m_str, str.c_str(), len);
			m_str[len] = NULL;
		}

		virtual ~String() {
		}

		wchar_t* Format(const wchar_t* fmt, ...) {
			va_list args;
			va_start(args, fmt);
			vswprintf_s(m_str, fmt, args);
			va_end(args);
			return m_str;
		}

		//------------------------------------------------------------------------------------
		// shlwapi
		bool IsRelativePath() const {
			return PathIsRelative(m_str) ? true : false;
		}

		bool IsFileExist() const {
			return common::IsFileExist(m_str);
		}

		String convertToString(double num) const {
			String str;
			swprintf_s(str.m_str, L"%.1f", num);
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
			if (_wstat64(m_str, &buf) != 0)
				return -1; // error, could use errno to find out more
			return buf.st_size;
			return 0;
		}

		String FileSizeStr() const {
			return convertSize((size_t)FileSize());
		}

		const wchar_t* GetFileExt() const {
			return PathFindExtensionW(m_str);
		}

		const wchar_t* GetFileName() const {
			return PathFindFileNameW(m_str);
		}

		String GetFullFileName() const {
			String str;
			if (IsRelativePath())
			{
				wchar_t curDir[MAX];
				GetCurrentDirectory(MAX, curDir);
				wcscat_s(curDir, L"/");
				wcscat_s(curDir, m_str);
				GetFullPathName(curDir, MAX, str.m_str, NULL);
			}
			else
			{
				GetFullPathName(m_str, MAX, str.m_str, NULL);
			}
			return str;
		}

		String GetFilePathExceptFileName() const {
			String str = *this;
			PathRemoveFileSpec(str.m_str);
			return str;
		}

		// only return filename except extends
		String GetFileNameExceptExt() const {
			String str = PathFindFileName(m_str);
			PathRemoveExtension(str.m_str);
			return str;
		}

		// return full filename except extends
		String GetFileNameExceptExt2() const {
			String str = *this;
			wchar_t *name = PathFindFileName(str.m_str);
			PathRemoveExtension(name);
			return str;
		}


		//------------------------------------------------------------------------------------
		// STL
		void erase(const wchar_t *str) {
			wchar_t *p = (wchar_t*)find(str);
			if (!p || !*p)
				return;

			std::rotate(p, p + wcslen(str), m_str + wcslen(m_str));
			const int len = wcslen(m_str) - wcslen(str);
			m_str[len] = NULL;
		}

		const wchar_t* find(const wchar_t *str) const {
			const wchar_t *p = m_str;
			while (*p)
			{
				const wchar_t *n = p;
				const wchar_t *c = str;
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

		const wchar_t* find(const String &str) const {
			return find(str.m_str);
		}

		wchar_t back() {
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
			std::transform(m_str, &m_str[MAX], m_str, _wcslwr);
			return *this;
		}

		String& upperrCase() {
			std::transform(m_str, &m_str[MAX], m_str, _wcsupr);
			return *this;
		}

		hashcode GetHashCode() const {
			boost::hash<std::wstring> string_hash;
			return string_hash(m_str);
		}

		size_t size() const {
			return (size_t)wcslen(m_str);
		}

		void clear() {
			memset(m_str, 0, sizeof(wchar_t)*MAX);
		}
		//------------------------------------------------------------------------------------

		//------------------------------------------------------------------------------------
		// ETC
		//std::string str() const
		//{
		//	const int slength = (int)size() + 1;
		//	int len = ::WideCharToMultiByte(CP_ACP, 0, m_str, slength, 0, 0, NULL, FALSE);
		//	char buf[MAX];
		//	ZeroMemory(buf, sizeof(buf));
		//	len = min(MAX - 1, len);
		//	::WideCharToMultiByte(CP_ACP, 0, m_str, slength, buf, len, NULL, FALSE);
		//	std::string r(buf);
		//	return r;
		//}

		String<char,MAX> str() const
		{
			const int slength = (int)size() + 1;
			int len = ::WideCharToMultiByte(CP_ACP, 0, m_str, slength, 0, 0, NULL, FALSE);
			String<char, MAX> buf;
			len = min(MAX - 1, len);
			::WideCharToMultiByte(CP_ACP, 0, m_str, slength, buf.m_str, len, NULL, FALSE);
			return buf;
		}

		// Default Ansi - UTF-8
		String<char, MAX> utf8() const
		{
			const int slength = (int)size() + 1;
			int len = ::WideCharToMultiByte(CP_UTF8, 0, m_str, slength, 0, 0, NULL, FALSE);
			String<char, MAX> buf;
			len = min(MAX - 1, len);
			::WideCharToMultiByte(CP_UTF8, 0, m_str, slength, buf.m_str, len, NULL, FALSE);
			return buf;
		}

		// UTF-8 -> Default Ansi
		String<char, MAX> ansi() const
		{
			const int slength = (int)size() + 1;
			int len = ::WideCharToMultiByte(CP_ACP, 0, m_str, slength, 0, 0, NULL, FALSE);
			String<char, MAX> buf;
			len = min(MAX - 1, len);
			::WideCharToMultiByte(CP_ACP, 0, m_str, slength, buf.m_str, len, NULL, FALSE);
			return buf;
		}
		//------------------------------------------------------------------------------------


		const wchar_t* c_str() const { return (const wchar_t*)m_str; }

		bool operator == (const String &rhs) const {
			return !wcscmp(m_str, rhs.m_str);
		}

		bool operator == (const std::wstring &rhs) const {
			return rhs == m_str;
		}

		bool operator == (const wchar_t *str) const {
			return !wcscmp(m_str, str);
		}

		bool operator != (const String &rhs) const {
			return !(operator==(rhs));
		}

		bool operator != (const std::wstring &rhs) const {
			return !(operator==(rhs));
		}

		bool operator != (const wchar_t *str) const {
			return !(operator==(str));
		}


		String& operator = (const wchar_t *str) {
			if (!str)
				return *this;

			if (m_str != str)
			{
				const size_t len = min(wcslen(str), MAX - 1);
				wcsncpy_s(m_str, str, len);
				m_str[len] = NULL;
			}
			return *this;
		}

		String& operator = (const String &rhs) {
			if (this != &rhs) {
				operator = (rhs.m_str);
			}
			return *this;
		}

		String& operator = (const std::wstring &rhs) {
			return operator = (rhs.c_str());
		}

		String operator + (const wchar_t *str) const {
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

		String& operator += (const wchar_t *str) {
			const size_t len1 = wcslen(m_str);
			if (len1 >= (MAX - 1))
				return *this;

			const size_t len2 = wcslen(str);
			const size_t cpLen = min(len2, MAX - len1 - 1);
			wcsncat_s(m_str, str, cpLen);
			m_str[len1 + cpLen] = NULL;
			return *this;
		}

		String& operator += (const wchar_t c) {
			const size_t len1 = wcslen(m_str);
			if ((len1+1) >= (MAX - 1))
				return *this;

			m_str[len1] = c;
			m_str[len1 + 1] = NULL;
			return *this;
		}


		String& operator += (const String &str) {
			if (this == &str)
				return *this;

			const size_t len1 = wcslen(m_str);
			if (len1 >= (MAX - 1))
				return *this;

			const size_t len2 = wcslen(str.m_str);
			const size_t cpLen = min(len2, MAX - len1 - 1);
			wcsncat_s(m_str, str.m_str, cpLen);
			m_str[len1 + cpLen] = NULL;
			return *this;
		}

		bool operator < (const String &rhs) const {
			return std::wcscmp(m_str, rhs.m_str) < 0;
		}


	public:
		wchar_t m_str[MAX];
		const size_t SIZE = MAX;
	};



	// Define Type
	typedef String<char, 16> Str16;
	typedef String<char, 32> Str32;
	typedef String<char, 64> Str64;
	typedef String<char, 64> StrId;
	typedef String<char, 128> Str128;
	typedef String<char, 256> Str256;
	typedef String<char, 512> Str512;
	typedef String<char, 128> StrPath;
	typedef String<char, 256> StrGlobalPath;

	typedef String<wchar_t, 16> WStr16;
	typedef String<wchar_t, 32> WStr32;
	typedef String<wchar_t, 64> WStr64;
	typedef String<wchar_t, 64> WStrId;
	typedef String<wchar_t, 128> WStr128;
	typedef String<wchar_t, 256> WStr256;
	typedef String<wchar_t, 512> WStr512;
	typedef String<wchar_t, 128> WStrPath;
	typedef String<wchar_t, 256> WStrGlobalPath;
}
