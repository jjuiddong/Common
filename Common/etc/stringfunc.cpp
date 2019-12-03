
#include "stdafx.h"
#include "stringfunc.h"


void common::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

// "  skdfjskdjf  "
// "$$$skdfjskdjf$$$"
// "skdfjskdjf"
string& common::trim(string &str)
{
	// 앞에서부터 검색
	for (int i=0; i < (int)str.length(); ++i)
	{
		if ((str[i] == '\n') || (str[i] == '\t') || (str[i] == '\r') || (str[i] == ' '))
		{
			rotatepopvector(str, i);
			--i;
		}
		else
			break;
	}

	// 뒤에서부터 검색
	for (int i=(int)str.length()-1; i >= 0; --i)
	{
		if ((str[i] == '\n') || (str[i] == '\t') || (str[i] == '\r') || (str[i] == ' '))
		{
			rotatepopvector(str, i);
		}
		else
			break;
	}

	return str;
}


// "  skdfjskdjf  "
// "$$$skdfjskdjf$$$"
// "skdfjskdjf"
void common::trimw(wstring &str)
{
	str = str2wstr(trim(wstr2str(str)));
}


//------------------------------------------------------------------------
// 유니코드를 멀티바이트 문자로 변환
//------------------------------------------------------------------------
std::string common::wstr2str(const std::wstring &wstr)
{
	const int slength = (int)wstr.length() + 1;
	const int len = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), slength, 0, 0, NULL, FALSE);
	char* buf = new char[len];
	::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), slength, buf, len, NULL, FALSE);
	std::string r(buf);
	delete[] buf;
	return r;
}


//------------------------------------------------------------------------
// 멀티바이트 문자를 유니코드로 변환
//------------------------------------------------------------------------
std::wstring common::str2wstr(const std::string &str)
{
	int len;
	int slength = (int)str.length() + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}


string common::wstr2utf8(const wstring &wstr)
{
	const int slength = (int)wstr.length() + 1;
	const int len = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), slength, 0, 0, NULL, FALSE);
	char* buf = new char[len];
	::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), slength, buf, len, NULL, FALSE);
	std::string r(buf);
	delete[] buf;
	return r;
}


//------------------------------------------------------------------------
// _variant_t 타입을 스트링으로 변환시킨다. 데이타 출력용을 만들어졌다.
// isStringDoubleQuoto: 스트링 타입일 경우, 양쪽에 쌍따옴표로 묶는다.
//------------------------------------------------------------------------
std::string common::variant2str(const _variant_t &var
	, const bool isStringDoubleQuoto //= false
)
{
	std::stringstream ss;
	switch (var.vt)
	{
	case VT_BOOL: ss << var.boolVal; break;
	case VT_I2: ss << var.iVal; break;
	case VT_I4: ss << var.lVal; break;
	case VT_R4: ss << var.fltVal; break;
	case VT_R8: ss << var.dblVal; break;

	case VT_BSTR:
	{
#ifdef _UNICODE
		wstring str = (LPCTSTR)(_bstr_t)var.bstrVal;
		if (isStringDoubleQuoto)
		{
			ss << "\"" << common::wstr2str(str) << "\"";
		}
		else
		{
			ss << common::wstr2str(str);
		}
#else
		string str = (LPCTSTR)(_bstr_t)var.bstrVal;
		if (isStringDoubleQuoto)
		{
			ss << "\"" << str << "\"";
		}
		else
		{
			ss << str;
		}
#endif
	}
	break;

	case VT_DECIMAL:
	case VT_I1:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
		break;

	case VT_INT: ss << var.intVal; break;
	case VT_UINT: ss << var.uintVal; break;
	default:
		break;
	}

	return ss.str();
}


//------------------------------------------------------------------------
// string을 varType 형태로 변환해서 리턴한다.
// VT_BSTR 타입일 경우 ::SysAllocString()로 생성하고,
// ::SysFreeString() 으로 해제해야 한다.
// https://manylee.tistory.com/entry/VariantCopy-VariantInit-VariantClear
// https://panpro.tistory.com/75
//------------------------------------------------------------------------
_variant_t common::str2variant(const VARTYPE &vt, const std::string &value)
{
	_variant_t var;
	var.vt = vt;
	switch (vt)
	{
	case VT_BOOL: var.boolVal = (bool)atoi(value.c_str()); break;
	case VT_I2: var.iVal = (short)atoi(value.c_str()); break;
	case VT_I4: var.lVal = (long)atoi(value.c_str()); break;
	case VT_R4: var.fltVal = (float)atof(value.c_str()); break;
	case VT_R8: var.dblVal = atof(value.c_str()); break;

	case VT_BSTR:
	{
#ifdef _UNICODE
		//var.bstrVal = (_bstr_t)common::str2wstr(value).c_str();
		var.bstrVal = ::SysAllocString(common::str2wstr(value).c_str());
#else
		var.bstrVal = ::SysAllocString(common::str2wstr(value).c_str());
#endif
	}
	break;

	case VT_DECIMAL:
	case VT_I1:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
		break;

	case VT_INT: var.intVal = (int)atoi(value.c_str()); break;
	case VT_UINT: var.uintVal = strtoul(value.c_str(), NULL, 0); break;
	default:
		return false;
	}
	return var;
}


// Deep Copy variant
// VT_BSTR 타입일 경우 복사해서 리턴한다.
_variant_t common::copyvariant(const _variant_t &var)
{
	_variant_t v;
	if (VT_BSTR == var.vt)
	{
		v.vt = var.vt;
		v.bstrVal = ::SysAllocString(var.bstrVal);
	}
	else
	{
		v = var;
	}
		
	return v;
}


// VT_BSTR 타입일 경우 ::SysFreeString() 으로 해제해야 한다.
// https://manylee.tistory.com/entry/VariantCopy-VariantInit-VariantClear
// https://panpro.tistory.com/75
void common::clearvariant(INOUT _variant_t &var)
{
	if (VT_BSTR == var.vt)
	{
		::SysFreeString(var.bstrVal);
		var.vt = VT_EMPTY;
	}
	else
	{
		::VariantInit(&var);
	}
}


//------------------------------------------------------------------------
// 스트링포맷
//------------------------------------------------------------------------
std::string common::format(const char* fmt, ...)
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );
	return textString;
}



//------------------------------------------------------------------------
// 스트링포맷 wstring 용
//------------------------------------------------------------------------
std::wstring common::formatw(const char* fmt, ...)
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );
	return str2wstr(textString);
}


// 넘어온 인자 str 을 소문자로 바꿔서 리턴한다.
string& common::lowerCase(string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
}

// 넘어온 인자 str 을 대문자로 바꿔서 리턴한다.
string& common::upperCase(string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper);
	return str;
}


wstring& common::lowerCasew(wstring &str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
}

wstring& common::upperCasew(wstring &str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper);
	return str;
}


// 원본 문자열 str에서 구분자 delimeter로 분해해서 out 에 저장해서 리턴한다.
// delimeter 는 저장되지 않는다.
void common::tokenizer(const string &str, const string &delimeter
	, const string &ignoreStr, OUT vector<string> &out)
{
	string tmp = str;
	int offset = 0;
	int first = 0;

	while (!tmp.empty())
	{
		const int pos = (int)tmp.find(delimeter, offset);
		if (string::npos == pos)
		{
			out.push_back(tmp.substr(first));
			break;
		}
		else
		{
			const string tok = tmp.substr(offset, pos-offset);
			offset += (int)tok.length() + (int)delimeter.length();
			if (tok != ignoreStr)
			{
				out.push_back(tmp.substr(first, pos-first));
				first = offset;
			}
		}
	}
}


// 원본 문자열 str에서 구분자 delimeter 로 분해해서 out 에 저장해서 리턴한다.
// delimeter 는 저장되지 않는다.
void common::wtokenizer(const wstring &str, const wstring &delimeter, const wstring &ignoreStr, OUT vector<wstring> &out)
{
	wstring tmp = str;
	int offset = 0;
	int first = 0;

	while (!tmp.empty())
	{
		const int pos = (int)tmp.find(delimeter, offset);
		if (wstring::npos == pos)
		{
			out.push_back(tmp.substr(first));
			break;
		}
		else
		{
			const wstring tok = tmp.substr(offset, pos - offset);
			offset += (int)tok.length() + (int)delimeter.length();
			if (tok != ignoreStr)
			{
				out.push_back(tmp.substr(first, pos - first));
				first = offset;
			}
		}
	}
}


// 원본 문자열 str에서 구분자 delimeters 로 분해해서 out 에 저장해서 리턴한다.
// 이 때 구분 단위는 delimeters 의 한 문자를 기준으로 한다.
// delimeters 는 저장되지 않는다.
//
// ex) tokenizer2( "aaa; bbbb cccc ddd; eee", ", ", out)
//		out-> aaa bbb ccc ddd eee
void common::tokenizer2(const string &str, const string &delimeters, OUT vector<string> &out)
{
	string tmp = str;
	int offset = 0;

	while (tmp[offset])
	{
		string tok;
		tok.reserve(32);
		while (tmp[offset])
		{
			if (strchr(delimeters.c_str(), tmp[offset]))
			{
				offset++;
				break;
			}
			tok += tmp[offset++];
		}

		if (!tok.empty())
			out.push_back(tok);
	}
}


// delimeter로 구분된 스트링을 파싱해서 int형으로 변환한 후 리턴한다.
// 동적메모리 생성, 메모리 복사를 최소화하기 위해 따로 만들었다.
void common::tokenizer3(const char *data, const int size, const char delimeter, OUT vector<int> &out)
{
	char buff[32];
	const char *src = data;
	char *dst = buff;
	int remain = size;
	while (remain > 0)
	{
		if ((*src == '\0') 
			|| (*src == delimeter)
			|| ((int)(dst - buff) > (ARRAYSIZE(buff)-1))
			)
		{
			*dst++ = NULL;
			out.push_back(atoi(buff));
			dst = buff;
		}
		else
		{
			*dst++ = *src;
		}
		++src;
		--remain;
	}

	// 마지막 토큰 저장, 저장한 정보가 있다면 ~
	if ((int)(dst - buff) > 0)
	{
		// null 추가
		if ((int)(dst - buff) < ARRAYSIZE(buff))
		{
			*dst++ = NULL;
			out.push_back(atoi(buff));
		}
	}
}


// 공백문자와 (space, newline, tab) 쌍따옴표(double quote), 콤마(,)를 
// 기준으로 문자를 구분한다.
// 쌍따옴표 사이의 문자는 어떤 문자든지 (delimeter 여부와 상관없이) 문자로 인식한다.
void common::tokenizer_space(const string &str, OUT vector<string> &out)
{
	int state = 0;
	string tok;
	const char *c = str.c_str();
	while (*c)
	{
		if (state == 0)
		{
			switch (*c)
			{
			case '\n':
			case ' ':
			case '\t':
			case '\r':
			case ',':
				if (!tok.empty())
				{
					out.push_back(tok);
					tok.clear();
				}
				break;

			case '\"':
				state = 1;
				break;

			default: 
				tok += *c;
				break;
			}
		}
		else if (state == 1)
		{
			if (*c == '\"')
			{
				if (!tok.empty())
				{
					out.push_back(tok);
					tok.clear();
					state = 0;
				}
			}
			else
			{
				tok += *c;
			}
		}
		else
		{
			assert(0);
		}

		++c;
	}

	if (!tok.empty())
		out.push_back(tok);
}


// int형 배열을 delimeter를 구분자로해서 스트링으로 변환한다.
// 동적 메모리생성을 최소화한 함수.
// datas -> string 
int common::strcomposite(char *buff, const int maxSize, const char delimeter, const vector<int> &datas)
{
	char temp[32];
	char *dst = buff;
	int remain = maxSize;
	for (int i=0; i < (int)datas.size(); ++i)
	{
		auto &data = datas[i];
		sprintf_s(temp, "%d", data);
		const int len = strlen(temp);
		if (len < remain)
		{
			strncpy(dst, temp, len);
			dst += len;
			remain -= len;
		}

		if ((remain > 1) && (i != (int)datas.size()-1))
		{
			*dst++ = delimeter;
			--remain;
		}
	}

	if (remain >= 1)
	{
		*dst++ = NULL;
	}
	return maxSize - remain;
}


// explicit all matching character
// %f, %d
// Last Variable Argument Must NULL
//
// etc)
//  scanner("test string 1-1", "test string %d-%d", &v1, &v2, NULL);
//
int common::scanner(const wchar_t *buf, const wchar_t *fmt, ...)
{
	va_list listPointer;
	va_start(listPointer, fmt);

	const wchar_t *pfL = fmt;
	const wchar_t *pfR = fmt;
	const wchar_t *pb = buf;
	int scanCount = 0;

	while (*pfR && *pb)
	{
		if (*pfR == '%')
		{
			const int cmpLen = pfR - pfL;
			if (cmpLen > 0)
				if (wcsncmp(pb, pfL, cmpLen))
					break; // not matching fail Scan

			pb += cmpLen;
			const wchar_t f = *++pfR; // next
			pfL = ++pfR; // next formatter

			switch (f)
			{
			case 'd': // integer
			{
				wchar_t tmp[32];
				int idx = 0;
				ZeroMemory(tmp, sizeof(tmp));

				while (*pb && (31 > idx))
				{
					if (!iswdigit(*pb))
						break;
					tmp[idx++] = *pb++;
				}

				void *arg = va_arg(listPointer, void*);
				if (!arg)
					break; // no argument

				*(int*)arg = _wtoi(tmp);
				++scanCount;
			}
			break;

			case 'f': // floating
			{
				wchar_t tmp[32];
				int idx = 0;
				ZeroMemory(tmp, sizeof(tmp));

				while (*pb && (31 > idx))
				{
					if ((*pb == '.') && (!iswdigit(*pb)))
						break;
					tmp[idx++] = *pb++;
				}

				void *arg = va_arg(listPointer, void*);
				if (!arg)
					break; // no argument

				*(float*)arg = (float)_wtof(tmp);
				++scanCount;
			}
			break;

			default: assert(0); break;
			}
		}
		else
		{
			++pfR;
		}
	}

	va_end(listPointer);

	if (!*pfR && *pfL)
	{
		if (!wcscmp(pfL, pb))
			return scanCount; // success
	}

	if (!*pfR && !*pb)
		return scanCount; // success

	return -1; // fail
}
