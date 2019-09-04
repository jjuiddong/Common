
#include "stdafx.h"
#include "config.h"
#include <set>

using namespace common;
using namespace std;


cConfig::cConfig( const string &fileName //= ""
)
	: m_program(NULL)
{
	m_keywords.push_back({ sReservedKeyword::IFEQ, "ifeq" });
	m_keywords.push_back({ sReservedKeyword::IFNEQ, "ifneq" });
	m_keywords.push_back({ sReservedKeyword::ELSE, "else" });
	m_keywords.push_back({ sReservedKeyword::ENDIF, "endif" });

	if (!fileName.empty())
		Read(fileName);
}

cConfig::~cConfig()
{
	Remove(m_program);
}


bool cConfig::Read(const string &fileName)
{
	m_options.clear();

	if (!Parse(fileName))
	{
		InitDefault();
		return false;
	}
	return true;
}


// 환경설정 파일을 읽어서 저장한다.
bool cConfig::Parse(const string &fileName)
{
	std::ifstream cfgfile(fileName);
	if (!cfgfile.is_open())
	{
		printf("not exist <%s> config file!!\n", fileName.c_str());
		return false;
	}

	CheckUTF8withBOM(cfgfile);

	if (IsMakeFileSyntax(fileName))
	{
		Remove(m_program);
		m_program = program(cfgfile);
		Execute(m_program);
		return true;
	}

	Str512 tok, eq, val;
	int state = 0; // -1:finish
	while (!cfgfile.eof())
	{
		int state = 0;
		if (!GetToken(cfgfile, tok, false, false, &state))
		{
			if (state == 1) // eof?
				break;
			else
				continue;
		}

		switch (state)
		{
		case 0:
		{
			sReservedKeyword::TYPE type = GetReservedKeyword(tok.c_str());
			switch (type)
			{
			case sReservedKeyword::NONE:
			{
				if (!GetToken(cfgfile, eq))
					break;

				if (eq == "=")
				{
					if (GetToken(cfgfile, val, false, true))
						m_options[tok.c_str()] = val.c_str();
				}
				else
				{
					dbg::Logc(2, "error read config file = [ %s ]\n", fileName.c_str());
					state = -1;
				}
			}
			break;

			case sReservedKeyword::IFEQ:
				break;
			case sReservedKeyword::IFNEQ:
				break;
			case sReservedKeyword::ELSE:
				break;
			case sReservedKeyword::ENDIF:
				break;
			default:
				dbg::Logc(2, "cConfig::Parse() no keyword type\n");
				break;
			}
		}
		break;

		case 1:
			break;
		}
	}
	

	//string id, eq, val;
	//while (cfgfile >> id)
	//{
	//	val.clear();

	//	if (id[0] == '#')
	//	{
	//		string line;
	//		getline(cfgfile, line);
	//		continue;  // comments, skip
	//	}
	//	else if (id == "plot_command")
	//	{
	//		cfgfile >> eq;

	//		// read all file
	//		std::string strCmd((std::istreambuf_iterator<char>(cfgfile)), std::istreambuf_iterator<char>());
	//		common::trim(strCmd);
	//		m_options[id] = strCmd;
	//	}
	//	else
	//	{
	//		cfgfile >> eq >> val;
	//		if (eq != "=") throw std::runtime_error("Parse error");
	//		m_options[id] = val;
	//	}
	//}

	UpdateParseData();

	return true;
}


// parse stream, return token
// isTokenizingSpecialChar: if true, special character detect = ,
// state: eof state 1
bool cConfig::GetToken(std::istream &stream, OUT Str512 &out
	, const bool isTokenizingSpecialChar // =false
	, const bool isScanSpacing // =false
	, OUT int *eofState // =NULL
) 
{
	out.m_str[0] = NULL;
	if (stream.eof())
		return false;

	int state = 0; // 0:ignore space, 1:parse, -1:finish
	char *dst = out.m_str;
	while (!stream.eof() 
		&& (-1 != state)
		&& (dst - out.m_str < (int)out.SIZE - 1))
	{
		if ((1 == state) 
			&& ((stream.peek() == '\n') 
				|| (stream.peek() == '\r') 
				|| (!isScanSpacing && (stream.peek() == ' '))
				|| (stream.peek() == '\t')))
			break; // end of line

		char c = stream.get();

		if (c == -1) // eof?
		{
			if (eofState)
				*eofState = 1;
			break;
		}

		if ((state == 0) && ((' ' == c) || ('\t' == c) || (NULL == c) || ('\n' == c))) // ignore space mode?
			continue;

		switch (c)
		{
		case '#': // comment?
		{
			// read until newline
			string str;
			while (!stream.eof())
			{
				c = NULL;
				stream.read(&c, sizeof(c));
				if (!c || (c == '\n') || (c == '\r'))
					break;
				str += c;
			}

			if (dst == out.m_str) // empty?
			{
				// continue parsing
			}
			else
			{
				state = -1; // finish
			}
		}
		break;
		
		case '=':
		case '(':
		case ')':
		{
			if (dst == out.m_str) // empty?
			{
				*dst++ = c;
			}
			else
			{
				const std::streampos pos = stream.tellg();
				stream.seekg(pos - std::streampos(1), stream.beg); // goto back
			}
			state = -1; // finish
		}
		break;

		default:
		{
			if (isTokenizingSpecialChar && (c == ','))
			{
				if (dst == out.m_str) // empty?
				{
					*dst++ = c;
				}
				else
				{
					const std::streampos pos = stream.tellg();
					stream.seekg(pos - std::streampos(1), stream.beg); // goto back
				}
				state = -1; // finish
			}
			else
			{
				state = 1;
				*dst++ = c;
			}
		}
		break;
		}
	}

	*dst = NULL;
	return !out.empty();
}


cConfig::sReservedKeyword::TYPE cConfig::GetToken2(std::istream &stream, OUT Str512 &out
	, const bool isTokenizingSpecialChar //= false
)
{
	if (!GetToken(stream, out, isTokenizingSpecialChar))
		return sReservedKeyword::EOF0;
	return GetReservedKeyword(out.c_str());
}


// back to stream token size
void cConfig::UnGetToken(std::istream &stream, const Str512 &tok)
{
	const std::streampos pos = stream.tellg();
	stream.seekg(pos - std::streampos(tok.size()), stream.beg);
}


// return reserved keyword, if exist
cConfig::sReservedKeyword::TYPE cConfig::GetReservedKeyword(const char *str)
{
	auto it = std::find_if(m_keywords.begin(), m_keywords.end()
		, [&](auto &a) {return a.name == str; });
	if (m_keywords.end() == it)
		return sReservedKeyword::NONE;
	return it->type;
}


// 입력파일이 makefile 문법이라면 true를 리턴한다.
bool cConfig::IsMakeFileSyntax(const string &fileName)
{
	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;

	Str512 line;
	while (ifs.getline(line.m_str, line.SIZE))
	{
		if (line.find("ifeq"))
			return true;
		if (line.find("ifneq"))
			return true;
	}

	return false;
}


// key 값에 해당하는 value를 리턴한다.
// 없다면, key값이 value가 된다.
// key : $varname
//		 첫문자가 $일때, 변수로 처리함.
//		 아니라면, 값으로 처리
// 재귀적으로 찾아 들어간다. 무한루프를 방지하기 위해서 100번까지만 검사한다.
const string& cConfig::GetSymbolData(const string &key
	, const int cnt //= 100
)
{
	if (cnt <= 0)
		return key;
	if (key.empty())
		return key;
	if (key[0] != '$')
		return key;

	const string realName = &key[1]; // except '$' string
	auto it = m_options.find(realName);
	if (m_options.end() == it)
		return key;
	return GetSymbolData(it->second, cnt-1);
}


// program -> expr-list
cConfig::sProgram* cConfig::program(std::istream &stream)
{
	sProgram *prog = new sProgram;
	prog->exprList = expr_list(stream);
	return prog;
}


// expr-list -> { expr }
cConfig::sExpr* cConfig::expr_list(std::istream &stream)
{
	sExpr *first = NULL;
	sExpr *prev = NULL;
	while (!stream.eof())
	{
		sExpr *p = expr(stream);
		if (!p)
			break;

		if (!first)
		{
			first = p;
			prev = p;
		}
		else
		{
			prev->next = p;
			prev = p;
		}
	}
	return first;
}


// expr -> if_expr | ifelse_expr | assign_expr
cConfig::sExpr* cConfig::expr(std::istream &stream)
{
	const sReservedKeyword::TYPE type = GetToken2(stream, m_tok);
	if (sReservedKeyword::EOF0 == type)
		return NULL;
	if (m_tok.empty())
		return NULL;

	sExpr *p = NULL;
	sReservedKeyword::TYPE endType = sReservedKeyword::NONE;

	switch (type)
	{
	case sReservedKeyword::IFEQ:
	case sReservedKeyword::IFNEQ:
		p = new sExpr;
		p->next = NULL;
		p->ifExpr = expr_if(stream, type);
		p->assignExpr = NULL;
		if (sReservedKeyword::ENDIF != GetToken2(stream, m_tok))
		{
			dbg::Logc(2, "error!! cConfig::expr(), not found endif\n");
			break;
		}
		break;

	case sReservedKeyword::NONE:
		p = new sExpr;
		p->next = NULL;
		p->ifExpr = NULL;
		p->assignExpr = expr_assign(stream, m_tok);
		if (!p->assignExpr)
			SAFE_DELETE(p);
		break;

	case sReservedKeyword::ELSE:
	case sReservedKeyword::ENDIF:
	{
		UnGetToken(stream, m_tok); // back to streampos
	}
	break;

	default:
		dbg::Logc(2, "cConfig::expr() type error \n");
		break;
	}

	return p;
}


// assign_expr -> id = var
cConfig::sAssignExpr* cConfig::expr_assign(std::istream &stream, const Str512 &tok)
{
	sAssignExpr *p = new sAssignExpr;
	p->id = tok.c_str();

	GetToken(stream, m_tok);
	if (m_tok != "=")
	{ 
		dbg::Logc(2, "config syntax error, [ %s ] not exist '='\n", p->id.c_str());
		delete p;
		return NULL;
	}

	p->var = variable(stream);

	return p;
}


// if_expr -> [ifeq | ifneq] ( var, var ) expr-list endif
// ifelse_expr -> [ifeq | ifneq] ( var, var ) expr-list 
//				    else [if_expr | ifelse_expr | expr-list endif]
cConfig::sIfExpr* cConfig::expr_if(std::istream &stream
	, const sReservedKeyword::TYPE type)
{
	if ((sReservedKeyword::IFEQ != type) &&
		(sReservedKeyword::IFNEQ != type))
		return NULL;

	sIfExpr *p = new sIfExpr;
	p->type = type;
	p->elseExprList = NULL;
	p->elseIfExpr = NULL;
	p->exprList = NULL;

	//Str512 a1, a2;
	sReservedKeyword::TYPE nextType = sReservedKeyword::NONE;
	sReservedKeyword::TYPE subType = sReservedKeyword::NONE;

	GetToken(stream, m_tok);
	if (m_tok != "(")
		goto error;

	//if (!GetToken(stream, a1, true))
	//	goto error;
	p->arg1 = variable(stream);
	if (!GetToken(stream, m_tok, true))
		goto error;
	if (m_tok != ",")
		goto error;
	p->arg2 = variable(stream);
	//if (!GetToken(stream, a2))
	//	goto error;
	if (!GetToken(stream, m_tok))
		goto error;
	if (m_tok != ")")
		goto error;

	//p->arg1 = a1.c_str();
	//p->arg2 = a2.c_str();
	p->exprList = expr_list(stream);

	nextType = GetToken2(stream, m_tok);
	switch (nextType)
	{
	case sReservedKeyword::ENDIF:
		UnGetToken(stream, m_tok); // back to streampos
		break;

	case sReservedKeyword::ELSE:
	{
		subType = GetToken2(stream, m_tok);
		sIfExpr *p2 = expr_if(stream, subType);
		if (p2)
		{
			p->elseIfExpr = p2;
		}
		else
		{
			UnGetToken(stream, m_tok); // back to streampos
			p->elseExprList = expr_list(stream);
		}
	}
	break;

	default:
		dbg::Logc(2, "cConfig Syntax Error, not found endif \n");
		break;
	}

	return p;


error:
	dbg::Logc(2, "cConfig Syntax Error, not found endif \n");
	delete p->exprList;
	delete p;
	return NULL;
}


// variable -> $(id)  | id
string cConfig::variable(std::istream &stream)
{
	if (!GetToken(stream, m_tok, true))
		return "";

	if (m_tok == "$")
	{
		if (!GetToken(stream, m_tok))
			return "";
		if (m_tok != "(")
			return "";
		Str512 var;
		if (!GetToken(stream, var))
			return "";
		if (!GetToken(stream, m_tok))
			return "";
		if (m_tok != ")")
			return "";

		return (Str512("$") + var).c_str();
	}
	else
	{
		return m_tok.c_str();
	}

	return {};
}


// execute makefile parse tree
void cConfig::Execute(sProgram *p)
{
	RET(!p);
	ExecuteExpr(p->exprList);
}


// execute expression
void cConfig::ExecuteExpr(sExpr *p)
{
	RET(!p);

	if (p->assignExpr)
	{
		m_options[p->assignExpr->id] = GetSymbolData(p->assignExpr->var);
	}
	else if (p->ifExpr)
	{
		ExecuteIfExpr(p->ifExpr);
	}

	ExecuteExpr(p->next);
}


// execute if expression
void cConfig::ExecuteIfExpr(sIfExpr *p)
{
	RET(!p);

	const string &var1 = GetSymbolData(p->arg1);
	const string &var2 = GetSymbolData(p->arg2);
	bool result = var1 == var2;

	if (p->type == sReservedKeyword::IFNEQ)
		result = !result;

	if (result)
	{
		ExecuteExpr(p->exprList);
	}
	else
	{
		if (p->elseIfExpr)
			ExecuteIfExpr(p->elseIfExpr);
		else
			ExecuteExpr(p->elseExprList);
	}
}


void cConfig::Remove(sProgram *p)
{
	RET(!p);
	Remove(p->exprList);
	delete p;
}


void cConfig::Remove(sExpr *p)
{
	RET(!p);
	Remove(p->next);
	Remove(p->ifExpr);
	delete p->assignExpr;
	delete p;
}


void cConfig::Remove(sIfExpr *p)
{
	RET(!p);
	Remove(p->exprList);
	Remove(p->elseIfExpr);
	Remove(p->elseExprList);
	delete p;
}


// 입력으로 들어온 스트링을 분석한다.
void cConfig::ParseStr(const string &str)
{
	using namespace std;

	stringstream ss(str);

	string id, eq, val;
	while (ss >> id)// >> eq >> val)
	{
		val.clear();

		if (id[0] == '#')
		{
			string line;
			getline(ss, line);
			continue;  // skip comments
		}
		else if (id == "plot_command")
		{
			ss >> eq;

			std::string strCmd((std::istreambuf_iterator<char>(ss)), std::istreambuf_iterator<char>());
 			common::trim(strCmd);
			m_options[id] = strCmd;
 		}
		else
		{
			ss >> eq;
			if (eq == "=")
			{
				ss >> val;
			}
			else
			{
				//ss >> val;
				const int pos = (int)eq.find('=');
				if (string::npos == pos)
				{
					dbg::Logc(2, "cConfig::Parse Error not equal '=' found \n");
				}
				else
				{
					val = eq.substr(pos+1);
				}
			}
			
			m_options[id] = val;
		}
	}

	UpdateParseData();
}


// if UTF-8 with BOM, ignore BOM (byte order mark)
// https://stackoverflow.com/questions/6769311/how-windows-notepad-interpret-characters
bool cConfig::CheckUTF8withBOM(std::ifstream &ifs)
{
	BYTE buff[3];
	ifs.read((char*)buff, sizeof(buff));

	if ((buff[0] == 0xEF)
		&& (buff[1] == 0xBB)
		&& (buff[2] == 0xBF))
	{
		return true;
	}
	else
	{
		// return to begin file pointer
		ifs.seekg(0, ifs.beg);
	}

	return false;
}


bool cConfig::GetBool(const string &key, const bool defaultValue)
{
	if (!CheckKey(key)) 
	{
		dbg::Logc(2, "error!! not found id = %s\n", key.c_str());
		return defaultValue;
	}

	if (m_options[key] == "true")
		return true;
	else if (m_options[key] == "false")
		return true;
	return (atoi(m_options[key].c_str()) > 0) ? true : false;
}


float cConfig::GetFloat(const string &key, const float defaultValue)
{
	if (!CheckKey(key)) 
	{
		dbg::Logc(2, "error!! not found id = %s\n", key.c_str());
		return defaultValue;
	}

	return (float)atof(m_options[key].c_str());
}


double cConfig::GetDouble(const string &key, const double defaultValue)
{
	if (!CheckKey(key)) 
	{
		dbg::Logc(2, "error!! not found id = %s\n", key.c_str());
		return defaultValue;
	}

	return atof(m_options[key].c_str());
}


int cConfig::GetInt(const string &key, const int defaultValue)
{
	if (!CheckKey(key)) 
	{
		dbg::Logc(2, "error!! not found id = %s\n", key.c_str());
		return defaultValue;
	}

	return atoi(m_options[key].c_str());
}


string cConfig::GetString(const string &key, const string &defaultValue) //defaultValue=""
{
	if (!CheckKey(key))
	{
		dbg::Logc(2, "error!! not found id = %s\n", key.c_str());
		return defaultValue;
	}

	return m_options[key];
}


// format = x, y, z
Vector3 cConfig::GetVector3(const string &key
	, const Vector3 &defaultValue //= Vector3(0, 0, 0)
)
{
	if (!CheckKey(key))
	{
		dbg::Logc(2, "error!! not found id = %s\n", key.c_str());
		return defaultValue;
	}

	string str = GetString(key);
	vector<string> strs;
	tokenizer(str, ";", "", strs);
	if (strs.size() < 3)
		return Vector3::Zeroes;

	const Vector3 ret(
		(float)atof(strs[0].c_str())
		, (float)atof(strs[1].c_str())
		, (float)atof(strs[2].c_str()));
	return ret;
}


// return true if exist key
bool cConfig::CheckKey(const string &key)
{
	auto it = m_options.find(key);
	return m_options.end() != it;
}


void cConfig::SetValue(const StrId &key, const char *value)
{
	m_options[key.c_str()] = value;
}

void cConfig::SetValue(const StrId &key, const int value)
{
	Str64 str;
	str.Format("%d", value);
	m_options[key.c_str()] = str.c_str();
}

void cConfig::SetValue(const StrId &key, const bool value)
{
	Str64 str;
	str.Format("%d", value);
	m_options[key.c_str()] = str.c_str();
}

void cConfig::SetValue(const StrId &key, const float value)
{
	SetValue(key, (double)value);
}


void cConfig::SetValue(const StrId &key, const double value)
{
	Str64 str;
	str.Format("%f", value);
	m_options[key.c_str()] = str.c_str();
}



// 동일한 이름에 파일이 있다면, 그 파일 형태를 그대로 유지하면서,
// 데이타만 업데이트 되는 형태로 저장하게 한다.
bool cConfig::Write(const string &fileName)
{
	if (m_program)
		return false; // script format not write

 	using namespace std;

	ifstream cfgfile(fileName);
	if (!cfgfile.is_open())
	{
		// 동일한 파일이 없다면, 그대로 저장하고 종료한다.
		ofstream ofs(fileName);
		if (!ofs.is_open())
			return false;

		for each (auto &kv in m_options)
		{
			if (kv.second.empty())
				ofs << kv.first << " = " << "0" << endl;
			else
				ofs << kv.first << " = " << kv.second << endl;
		}		
		return true;
	}


	set<string> checkId; //저장 된 정보와 그렇지 않은 정보를 찾기위해 필요함.

	stringstream ss;
	ss << cfgfile.rdbuf();

	stringstream savess;
	char line[128];
	while (ss.getline(line, sizeof(line)))
	{
		stringstream ssline(line);

		string id, eq, val;
		ssline >> id;

		if (id[0] == '#') // 주석
		{
			savess << line << endl;
			continue;
		}
		if (id.empty()) // 개행 문자
		{
			savess << line << endl;
			continue;
		}

		ssline >> eq >> val;
		if (eq != "=") throw std::runtime_error("Parse error");

		if (m_options[id].empty())
			savess << id << " = " << 0 << endl;
		else
			savess << id << " = " << m_options[id] << endl;

		checkId.insert(id);
	}

	// Write되지 않은 Id가 있다면, 검색해서, 저장한다.
	for each (auto &kv in m_options)
	{
		const auto it = checkId.find(kv.first);
		if (checkId.end() == it)
		{
			if (kv.second.empty())
				savess << kv.first << " = " << 0 << endl;
			else
				savess << kv.first << " = " << kv.second << endl;
		}
	}


	ofstream ofs(fileName);
	if (!ofs.is_open())
		return false;
	ofs << savess.rdbuf();

	return true;
}
