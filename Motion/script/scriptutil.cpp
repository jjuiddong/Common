
#include "stdafx.h"
#include "scriptutil.h"


namespace script
{
	const char *g_numStr = "1234567890.";
	const int g_numLen = 11;
	const char *g_strStr = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ@$_";
	const int g_strLen = 55;
	const char *g_strStr2 = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ@$_1234567890";
	const int g_strLen2 = 65;

	map<string, sFieldData> g_symbols; //{key, value} , key=variable name in script, value=data
	//cShmMap<script::sFieldData> g_symbols;
	bool g_isLog = false; // true 일 경우, 자세한 로그를 남긴다.
}


// 비교 연산자
// 같은 정보라면 true를 리턴한다.
bool script::sFieldData::operator == (const sFieldData &rhs)
{
	// 타입은 비교하지 않고, 정보만 비교한다.
	return (buff[0] == rhs.buff[0]) &&
		(buff[1] == rhs.buff[1]) &&
		(buff[2] == rhs.buff[2]) &&
		(buff[3] == rhs.buff[3]) &&
		(buff[4] == rhs.buff[4]) &&
		(buff[5] == rhs.buff[5]) &&
		(buff[6] == rhs.buff[6]) &&
		(buff[7] == rhs.buff[7]);
}


const script::sFieldData& script::sFieldData::operator=(const sFieldData &rhs)
{
	if (this != &rhs)
	{
		memcpy(buff, rhs.buff, sizeof(buff));
		type = rhs.type;
	}
	return *this;
}


void script::ClearSymbols()
{
	g_symbols.clear();
}


bool script::compare(string &src, char c)
{
	if (src.empty())
		return false;
	return src[0] == c;
}


bool script::check(string &src, char c)
{
	if (src.empty())
		return false;
	if (src[0] == c)
	{
		//rotatepopvector(src, 0);
		return true;
	}
	else
	{
		return false;
	}

	return true;
}


bool script::match(string &src, char c)
{
	if (src.empty())
		return false;
	if (src[0] == c)
	{
		rotatepopvector(src, 0);
	}
	else
	{
		dbg::ErrLog("script::check unmatch %c \n", c);
		return false;
	}

	return true;
}


// lex -> alphabet + {alphabet or number}
string script::id(string &src)
{
	trim(src);

	string str;

	// check start id token
	bool findFirstTok = false;
	for (int i = 0; i < g_strLen; ++i)
	{
		if (compare(src, g_strStr[i]))
		{
			findFirstTok = true;
			break;
		}
	}

	if (!findFirstTok)
		return "";

	str += src[0];
	rotatepopvector(src, 0);

	// check next id token
	while (!src.empty())
	{
		bool findtok = false;
		for (int i = 0; i < g_strLen2; ++i)
		{
			if (compare(src, g_strStr2[i]))
			{
				findtok = true;
				break;
			}
		}

		if (!findtok)
			break;

		str += src[0];
		rotatepopvector(src, 0);
	}

	return str;
}


// 한 라인을 리턴한다.
string script::line(string &src)
{
	string reval;
	int pos = src.find('\n');
	if (string::npos == pos)
	{
		reval = src;
		src.clear();
		return reval;
	}

	reval = src.substr(0, pos);
	for (int i=0; i < pos+1; ++i)
		rotatepopvector(src, 0);	
	return reval;
}


// lex -> [+,-] {number or .}
float script::number(string &src)
{
	trim(src);

	string str;
	if (src[0] == '+')
	{
		match(src, '+');
	}
	else if (src[0] == '-')
	{
		match(src, '-');
		str += '-';
	}

	while (!src.empty())
	{
		bool findtok = false;
		for (int i = 0; i < g_numLen; ++i)
		{
			if (compare(src, g_numStr[i]))
			{
				findtok = true;
				break;
			}
		}

		if (!findtok)
			break;

		str += src[0];
		rotatepopvector(src, 0);
	}

	const float n = (float)atof(str.c_str());
	return n;
}


// fmt -> %[number][.number][format]
// format : d,x,f
// ex : %2.3f
// string script::fmt(string &src)
// {	
// }



// change little endian -> big endian
void script::bigEndian(BYTE *buffer, const int size)
{
	// big endian
	int i = 0, k = size - 1;
	while (i < k)
	{
		std::swap(buffer[i], buffer[k]);
		++i;
		--k;
	}
}