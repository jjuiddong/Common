
#include "stdafx.h"
#include "plotinputparser.h"
#include "scriptutil.h"


cPlotInputParser::cPlotInputParser():
m_stmt(NULL)
{
}

cPlotInputParser::~cPlotInputParser()
{
	plotinputscript::rm_statement(m_stmt);
}


// read file and parsing
bool cPlotInputParser::Read(const string &fileName)
{
	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;

	std::string strCmd((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	common::trim(strCmd);
	if (!ParseStr(strCmd))
		return false;

	return true;
}


// read string and parsing
bool cPlotInputParser::ParseStr(const string &source)
{
	string src = source;

	plotinputscript::rm_statement(m_stmt);
	m_stmt = statement(src);
	return m_stmt? true : false;
}


// 실행
string cPlotInputParser::Execute()
{
	using namespace script;

	RETV(!m_stmt, "");

	string makeStr;

	plotinputscript::sStatement *p = m_stmt;
	while (p)
	{
		if (!p->symbol.empty())
		{
			auto it = g_symbols.find(p->symbol);
			if (g_symbols.end() != it)
			{
				switch (it->second.type)
				{
				case FIELD_TYPE::T_BOOL:
					makeStr += it->second.bVal ? "true" : "false";
					break;
				case FIELD_TYPE::T_SHORT:
				case FIELD_TYPE::T_INT:
					makeStr += format("%d", it->second.iVal);
					break;
				case FIELD_TYPE::T_UINT:
					makeStr += format("%u", it->second.uVal);
					break;
				case FIELD_TYPE::T_FLOAT:
					makeStr += format("%f", it->second.fVal);
					break;
				case FIELD_TYPE::T_DOUBLE:
					makeStr += format("%f", it->second.dVal);
					break;
				default:
					makeStr += "XXX";
					break;
				}
			}
			else
			{
				makeStr += "XXX";
			}
		}

		makeStr += p->str;
		p = p->next;
	}

	return makeStr;
}


// {[symbol] + [string]}
plotinputscript::sStatement* cPlotInputParser::statement(string &src)
{
	if (src.empty())
		return NULL;

	plotinputscript::sStatement *p = new plotinputscript::sStatement();
	p->symbol = symbol(src);
	p->str = str(src);
	p->next = statement(src);

	return p;
}


// $ 문자로 시작해서, 스페이스, 특수문자가 나올 때까지 읽어서 리턴한다. 
// symbol -> $or@{alphabet + number}
string cPlotInputParser::symbol(string &src)
{
	using namespace script;

	trim(src);

	if (src.empty())
		return "";

	// 모든 심볼들은 $,@부터 시작한다.
	if ((src[0] != '$') && (src[0] != '@'))
		return "";

	string sym;
	sym += src[0];

	match(src, src[0]);

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

		// 심볼하나에 $,@문자가 두번 들어올 수 없다.
		if ((src[0] == '$') || (src[0] == '@'))
			break;

		sym += src[0];
		rotatepopvector(src, 0);
	}

	return sym;
}


// lex -> {alphabet + space + special character} except $@
string cPlotInputParser::str(string &src)
{
	string ret;
	while (!src.empty())
	{
		if ((src[0] == '$') || (src[0] == '@'))
			break;

		ret += src[0];
		rotatepopvector(src, 0);
	}
	return ret;
}


// Plot Input String Format 을 파싱해서 저장한다.
// 2개 이상의 Format 스트링 파싱
void ParsePlotInputStringFormat(const string &str, OUT vector<cPlotInputParser> &out)
{
	stringstream ss(str);
	out.clear();
	out.reserve(4); // 최대 4개까지만 설정할 수 있다.

	// 한 라인씩 분석한다.
	int cnt = 0;
	char line[128];
	while (ss.getline(line, sizeof(line)) && (cnt < 4))
	{
		out.push_back(cPlotInputParser());
		out.back().ParseStr(line);
		++cnt;
	}
}

