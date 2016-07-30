
#include "stdafx.h"
#include "protocolparser.h"


cProtocolParser::cProtocolParser()
{
}

cProtocolParser::~cProtocolParser()
{
}


// 파일을 읽어서 파싱한다.
bool cProtocolParser::Read(const string &fileName, const string &delimeter)
{
	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;

	std::string strCmd((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	common::trim(strCmd);
	if (!ParseStr(strCmd, delimeter))
		return false;

	return true;
}


// 문자열을 읽어서 파싱한다.
bool cProtocolParser::ParseStr(const string &source, const string &delimeter)
{
	using namespace script;

	std::map<string, FIELD_TYPE::TYPE> typeMap;
	typeMap["short"] = FIELD_TYPE::T_SHORT;
	typeMap["int"] = FIELD_TYPE::T_INT;
	typeMap["uint"] = FIELD_TYPE::T_UINT;
	typeMap["float"] = FIELD_TYPE::T_FLOAT;
	typeMap["double"] = FIELD_TYPE::T_DOUBLE;
	typeMap["bool"] = FIELD_TYPE::T_BOOL;

	m_fields.clear();
	m_fields.reserve(64);

	vector<string> out;
	out.reserve(64);
	//common::tokenizer(source, "\r\n", "", out);
	common::tokenizer(source, delimeter, "", out);

	// debug log
	if (script::g_isLog)
	{
		dbg::Log("protocol parser input string \n");
		for each (auto &str in out)
		{
			dbg::Log("\t%s \n", str.c_str());
		}
		dbg::Log("\n");
	}

	for each (auto &str in out)
	{
		vector<string> tok;
		common::tokenizer(str, ",", "", tok);

		if (tok.size() >= 2)
		{
			sUDPField field;
			field.bytes = atoi(tok[0].c_str());
			
			// 주석을 뜻하는 #문자 이후의 문자를 무시한다.
			const int index1 = tok[1].find_first_of('#');
			string type;
			if (string::npos == index1)
				type = tok[1];
			else
				type = tok[1].substr(0, index1-1);

			const string t = trim(type);
			field.type = typeMap[t];

			if (field.type > 0)
			{ 
				m_fields.push_back(field);
			}
			else
			{
				//AfxMessageBox(CString(L"Protocol Parse Field Error --> ") + str2wstr(type).c_str());
				::MessageBoxA(NULL, format("Protocol Parse Field Error --> %s", type.c_str()).c_str(), "Error", MB_OK);
			}
		}
	}

	m_fieldsByteSize = 0;
	for each (auto &f in m_fields)
	{
		m_fieldsByteSize += f.bytes;
	}

	return true;
}

