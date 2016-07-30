
#include "stdafx.h"
#include "formatparser.h"
#include "scriptutil.h"

using namespace motion;
using namespace script;


cFormatParser::cFormatParser() 
	: m_stmt(NULL)
{
}

cFormatParser::~cFormatParser()
{
	rm_statement(m_stmt);
}


// 파일을 읽어서 파싱
bool cFormatParser::Read(const string &fileName)
{
	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;

	std::string strCmd((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	common::trim(strCmd);
	if (!Parse(strCmd))
		return false;
	return true;
}


// 문자열에서 파싱
bool cFormatParser::Parse(const string &str)
{
	string source = str;
	trim(source);
	if (source.empty())
		return true;

	m_stmt = statement(source);

	return true;
}


// m_stmt를 토대로 버퍼를 생성한다.
// Write된 버퍼 크기를 리턴한다.
int cFormatParser::Execute(BYTE *buffer, const int bufferSize)
{
	RETV(!m_stmt, 0);

	int len = 0;
	BYTE tempBuffer[128];

	sArgList *pargList = m_stmt->arglist;
	while (pargList && (len < bufferSize))
	{
		sArg *parg = pargList->arg;
		if (!parg)
			break;
		if (!parg->expr1)
			break;

		int cpySize = 0;

		switch (parg->expr1->type)
		{
		case ID:
		{
			auto it = g_symbols.find(parg->expr1->str);
			if (g_symbols.end() == it)
			{
				// 없는 심볼이라면 XXX 로 출력
				tempBuffer[0] = 'X';
				tempBuffer[1] = 'X';
				tempBuffer[2] = 'X';
				cpySize = 3;
			}
			else
			{
				if ((parg->expr2) && (FMT == parg->expr2->type))
				{
					// fmt 포맷에 맞게 형 변환 한 후 저장한다.
					const FIELD_TYPE::TYPE fmtType = GetFmtType(parg->expr2->str);				

					switch (it->second.type)
					{
					case FIELD_TYPE::T_BOOL:
						switch (fmtType)
						{
						case FIELD_TYPE::T_BOOL:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), it->second.bVal);
							break;
						case FIELD_TYPE::T_SHORT:
						case FIELD_TYPE::T_INT:
						case FIELD_TYPE::T_UINT:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (int)it->second.bVal);
							break;

						case FIELD_TYPE::T_FLOAT:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (float)it->second.bVal);
							break;
						case FIELD_TYPE::T_DOUBLE:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (double)it->second.bVal);
							break;

						default:
							assert(0);
							break;
						}
						break;

					case FIELD_TYPE::T_UINT:
						switch (fmtType)
						{
						case FIELD_TYPE::T_BOOL:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), it->second.uVal);
							break;
						case FIELD_TYPE::T_SHORT:
						case FIELD_TYPE::T_INT:
						case FIELD_TYPE::T_UINT:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), it->second.uVal);
							break;
						case FIELD_TYPE::T_FLOAT:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (float)it->second.uVal);
							break;
						case FIELD_TYPE::T_DOUBLE:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (double)it->second.uVal);
							break;
						default:
							assert(0);
							break;
						}
						break;

					case FIELD_TYPE::T_SHORT:
					case FIELD_TYPE::T_INT:
						switch (fmtType)
						{
						case FIELD_TYPE::T_BOOL:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), it->second.iVal);
							break;
						case FIELD_TYPE::T_SHORT:
						case FIELD_TYPE::T_INT:
						case FIELD_TYPE::T_UINT:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), it->second.iVal);
							break;
						case FIELD_TYPE::T_FLOAT:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (float)it->second.iVal);
							break;
						case FIELD_TYPE::T_DOUBLE:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (double)it->second.iVal);
							break;
						default:
							assert(0);
							break;
						}
						break;

					case FIELD_TYPE::T_FLOAT:
						switch (fmtType)
						{
						case FIELD_TYPE::T_BOOL:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (int)it->second.fVal);
							break;
						case FIELD_TYPE::T_SHORT:
						case FIELD_TYPE::T_INT:
						case FIELD_TYPE::T_UINT:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (int)it->second.fVal);
							break;
						case FIELD_TYPE::T_FLOAT:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), it->second.fVal);
							break;
						case FIELD_TYPE::T_DOUBLE:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (double)it->second.fVal);
							break;
						default:
							assert(0);
							break;
						}
						break;

					case FIELD_TYPE::T_DOUBLE:
						switch (fmtType)
						{
						case FIELD_TYPE::T_BOOL:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (int)it->second.dVal);
							break;
						case FIELD_TYPE::T_SHORT:
						case FIELD_TYPE::T_INT:
						case FIELD_TYPE::T_UINT:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (int)it->second.dVal);
							break;
						case FIELD_TYPE::T_FLOAT:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), (float)it->second.dVal);
							break;
						case FIELD_TYPE::T_DOUBLE:
							cpySize = sprintf_s((char*)tempBuffer, sizeof(tempBuffer), parg->expr2->str.c_str(), it->second.dVal);
							break;
						default:
							assert(0);
							break;
						}
						break;

					default:
						assert(0);
						break;
					}

					// 공백문자를 0으로 바꾼다.
					if (cpySize > 0)
					{
						for (int i = 0; i < cpySize; ++i)
						{
							if (' ' == tempBuffer[i])
								tempBuffer[i] = '0';
						}
					}					
				}
				else
				{
					switch (it->second.type)
					{
					case FIELD_TYPE::T_BOOL:
						tempBuffer[0] = it->second.bVal? 0 : 1;
						cpySize = sizeof(BYTE);
						break;
					case FIELD_TYPE::T_SHORT:
					case FIELD_TYPE::T_INT:
						memcpy(tempBuffer, &it->second.iVal, sizeof(int));
						cpySize = sizeof(int);
						break;
					case FIELD_TYPE::T_UINT:
						memcpy(tempBuffer, &it->second.uVal, sizeof(u_int));
						cpySize = sizeof(u_int);
						break;

					case FIELD_TYPE::T_FLOAT:
						memcpy(tempBuffer, &it->second.fVal, sizeof(float));
						cpySize = sizeof(float);
						break;
					case FIELD_TYPE::T_DOUBLE:
						memcpy(tempBuffer, &it->second.dVal, sizeof(double));
						cpySize = sizeof(double);
						break;

					default:
						assert(0);
						break;
					}
				}
			}
		}
		break;
		
		case STRING:  
		{
			// 문자열 복사.
			cpySize = (parg->expr1->str.size()  > sizeof(tempBuffer)) ? sizeof(tempBuffer) : parg->expr1->str.size();
			memcpy(tempBuffer, &parg->expr1->str[0], cpySize);
		}
		break;

		case FMT:
		default: assert(0); 
			break;
		}

		// 최종적으로 생성될 버퍼 buffer에 값을 저장한다.
		if (cpySize > 0)
		{
 			const int cpLen = ((cpySize + len) > bufferSize) ? (bufferSize - len) : cpySize;
			if (cpLen > 0)
			{
				memcpy(&buffer[len], tempBuffer, cpLen);
				len += cpLen;
			}
		}

		pargList = pargList->next;
	}

	return len;
}


// statement -> arg_list
cFormatParser::sStatement* cFormatParser::statement(string &src)
{
	trim(src);
	if (src.empty())
		return NULL;

	sStatement *p = new sStatement;
	p->arglist = arglist(src);
	return p;
}


// arg_list -> { arg }
cFormatParser::sArgList* cFormatParser::arglist(string &src)
{
	trim(src);

	sArg *parg = arg(src);
	if (!parg)
		return NULL;

	sArgList *pargList = new sArgList;
	pargList->arg = parg;
	pargList->next = arglist(src);
	return pargList;
}


// arg -> expr [, expr]
cFormatParser::sArg* cFormatParser::arg(string &src)
{
	trim(src);

	sExpr *pexpr1 = expr(src);
	if (!pexpr1)
		return NULL;

	sArg *parg = new sArg{ NULL, NULL };
	parg->expr1 = pexpr1;

	trim(src);
	if (src.empty())
		return parg;

	if (src[0] != ',')
		return parg;

	match(src, ',');

	parg->expr2 = expr(src);
	return parg;
}


// expr -> string | id | fmt | "string"
cFormatParser::sExpr* cFormatParser::expr(string &src)
{
	trim(src);

	if (src.empty())
		return NULL;

	// 모든 심볼들은 $,@부터 시작한다.
	if ((src[0] == '$') || (src[0] == '@'))
	{
		string sym = id(src);

		sExpr *pexpr = new sExpr;
		pexpr->str = sym;
		pexpr->type = cFormatParser::ID;
		return pexpr;
	}
	
	if (src[0] == '%') // fmt
	{
		sExpr *pexpr = new sExpr;
		pexpr->str = trim(line(src));
		pexpr->type = cFormatParser::FMT;
		return pexpr;
	}

	sExpr *pexpr = new sExpr;
	pexpr->str = trim(line(src));
	pexpr->type = cFormatParser::STRING;

	if (!pexpr->str.empty() && pexpr->str[0] == '\"') // " string "
		replaceAll(pexpr->str, "\"", "");// 쌍따옴표 제거

	pexpr->type = cFormatParser::STRING;

	return pexpr;
}


void cFormatParser::rm_statement(sStatement *p)
{
	RET(!p);
	rm_arglist(p->arglist);
	delete p;
}

void cFormatParser::rm_arglist(sArgList *p)
{
	RET(!p);
	rm_arg(p->arg);
	rm_arglist(p->next);
	delete p;
}

void cFormatParser::rm_arg(sArg *p)
{
	RET(!p);
	rm_expr(p->expr1);
	rm_expr(p->expr2);
	delete p;
}

void cFormatParser::rm_expr(sExpr *p)
{
	RET(!p);
	delete p;
}


script::FIELD_TYPE::TYPE cFormatParser::GetFmtType(const string &fmt)
{
	if (fmt.empty())
		return FIELD_TYPE::T_INT;

	const char c = fmt[fmt.size() - 1]; // last character
	switch (c)
	{
	case 'd':
		return FIELD_TYPE::T_INT;
	case 'x':
		return FIELD_TYPE::T_INT;
	case 'f':
		return FIELD_TYPE::T_FLOAT;
	default:
		assert(0);
		break;
	}
	
	return FIELD_TYPE::T_INT;
}
