#include "stdafx.h"
#include "ProtocolParser.h"

using namespace network2;


cProtocolParser::cProtocolParser()
{
	m_stmts = nullptr;
	m_isTrace = false;
	m_isError = false;
	m_isAutoRemove = true;
}

cProtocolParser::~cProtocolParser()
{
	if (m_isAutoRemove)
	{
		ReleaseStmt(m_stmts);
	}
}


//---------------------------------------------------------------------
// parse protocol script
//---------------------------------------------------------------------
sStmt* cProtocolParser::Parse( const string &fileName
	, bool isTrace //= false
	, bool isLog //= true
)
{
	if (!m_scan.LoadFile(fileName, isTrace))
		return nullptr;

	m_fileName = fileName;
	
	if (isLog)
		std::cout << fileName << " file Compile\n";

	m_token = m_scan.GetToken();
	if (ENDFILE == m_token)
	{
		m_scan.Clear();
		return nullptr;
	}

	m_stmts = stmt_list();

	if (ENDFILE != m_token)
	{
		SyntaxError(" code ends before file ");
		PrintToken(m_token, m_scan.GetTokenStringQ(0));
		m_scan.Clear();
		return nullptr;
	}

	// reconstruct statement parse tree
	// first statement have all protocol linked list, all type structure linked list
	{
		sStmt *s = new sStmt;
		sProtocol *pr = nullptr;
		sType *ty = nullptr;

		sStmt *p = m_stmts;
		while (p)
		{
			if (p->protocol)
			{
				if (pr)
				{
					pr->next = p->protocol;
					pr = pr->next;
				}
				else
				{
					s->protocol = p->protocol;
					pr = p->protocol;
				}
			}
			else if (p->type)
			{
				if (ty)
				{
					ty->next = p->type;
					ty = ty->next;
				}
				else
				{
					s->type = p->type;
					ty = p->type;
				}
			}
			p = p->next;
		}

		// remove statement only, no protocol, type
		p = m_stmts;
		while (p)
		{
			sStmt *t = p;
			p = p->next;
			delete t;
		}

		m_stmts = s;
	}

	return m_stmts;
}


// stmt_list -> (protocol | type)*
sStmt* cProtocolParser::stmt_list()
{
	sProtocol *p = nullptr;
	sType *t = nullptr;
	if (PROTOCOL == m_token)
	{
		p = protocol();
	}
	else if (TYPE == m_token)
	{
		t = type_stmt();
	}

	sStmt *stmt = nullptr;
	if (p || t)
	{
		stmt = new sStmt;
		stmt->protocol = p;
		stmt->type = t;
	}
	if (stmt)
		stmt->next = stmt_list();
	return stmt;
}


// protocol -> global id number [id] '{' packet_list '}'
sProtocol* cProtocolParser::protocol()
{
	sProtocol *p = nullptr;
	
	if (PROTOCOL == m_token)
	{
		Match(PROTOCOL);
		p = new sProtocol;
		p->name = id();
		p->number = num();
		p->next = nullptr;

		// protocol format (binary, ascii, json)
		if (ID == m_scan.GetTokenQ(0))
			p->format = id();

		Match(LBRACE);
		p->packet = packet_list();
		Match(RBRACE);
	}

	return p;
}


// type_stmt -> type id '{' decl_list '}'
sType* cProtocolParser::type_stmt()
{
	sType *p = nullptr;
	if (TYPE == m_token)
	{
		Match(TYPE);
		p = new sType;
		p->name = id();
		Match(LBRACE);
		p->vars = decl_list();
		Match(RBRACE);
	}
	return p;
}


// packet_list -> (packet)*
sPacket* cProtocolParser::packet_list()
{
	sPacket *p = packet();
	sPacket *first = p;
	while (p && (p->next = packet()))
	{
		p = p->next;
	}
	return first;
}


// packet -> [ packetid_stmt ] id '(' arg_list ')' semicolon
sPacket* cProtocolParser::packet()
{
	sPacket*p= nullptr;

	uint packetId = 0;
	if (LBRACE == m_token)
	{
		Match(LBRACE);
		if (id() == "packetid")
		{
			Match(COLON);
			packetId = (uint)num();
		}
		Match(RBRACE);
	}

	if (ID == m_token)
	{
		p = new sPacket;
		p->name = id();

		if (packetId == 0)
			p->packetId = common::StrId(p->name).GetHashCode();
		else
			p->packetId = packetId;

		Match(LPAREN);
		p->argList = arg_list();
		Match(RPAREN);
		p->next = nullptr;
	}

	if (p)
		Match(SEMICOLON);

	return p;
}


// arg_list -> [arg (',' arg)*]
sArg* cProtocolParser::arg_list()
{
	sArg *p = arg();
	if (!p)
		return nullptr;

	sArg *first = p;
	while (COMMA == m_token)
	{
		Match(COMMA);
		p->next = arg();
		p = p->next;
	}

	return first;
}


// decl_list -> decl [ decl_list ]
sArg* cProtocolParser::decl_list()
{
	sArg *p = decl();
	if (p)
		p->next = decl_list();
	return p;
}


// arg -> type
sArg* cProtocolParser::arg()
{
	sArg *p = nullptr;
	if (ID == m_token)
	{
		p = new sArg;
		p->var = type();
		p->next = nullptr;
	}
	return p;
}


// decl -> arg semicolon
sArg* cProtocolParser::decl()
{
	sArg *p = arg();
	if (p)
		Match(SEMICOLON);
	return p;
}


// type -> type_sub (var)?
sTypeVar* cProtocolParser::type()
{
	sTypeVar *p= nullptr;
	if (ID != m_token)
		return nullptr;

	eTokentype nextTok = m_scan.GetTokenQ(1);
	p = new sTypeVar;
	p->type = type_sub();
	p->var = var();

	return p;
}

// type_sub -> id '<' type_sub '>'
//			| id::id
//			| id
std::string cProtocolParser::type_sub()
{
	std::string str = "";

	if (ID == m_token)
	{
		eTokentype nextTok = m_scan.GetTokenQ(1);
		if (LT == nextTok)
		{
			str += id();
			str += "<";
			Match(LT);
			str += type_sub();
			str += ">";
			Match(RT);
		}
		else if (SCOPE == nextTok)
		{
			str += id();
			str += "::";
			Match(SCOPE);
			str += type_sub();
		}
		else
		{
			str += id();
		}
	}

	return str;
}

// var -> '*' id (index)?
//	    | '&' id (index)?
//		| id (index)?
//	    | '*'
//		| '&'
std::string cProtocolParser::var()
{
	std::string str = "";
	eTokentype nextTok = m_scan.GetTokenQ(1);

	if (TIMES == m_token && ID == nextTok)
	{
		Match(TIMES);
		str += "*";
		str += id();
		str += index();
	}
	else if (REF == m_token && ID == nextTok)
	{
		Match(REF);
		str += "&";
		str += id();
		str += index();
	}
	else if (ID == m_token)
	{
		str += id();
		str += index();
	}
	else if (TIMES == m_token)
	{
		Match(TIMES);
		str += "*";
	}
	else if (REF == m_token)
	{
		Match(REF);
		str += "&";
	}

	return str;
}

std::string cProtocolParser::index()
{
	std::string str = "";
	if (LBRACKET == m_token)
	{
		Match(LBRACKET);
		str += "[";
		str += number();
		str += "]";
		Match(RBRACKET);
	}
	return str;
}

std::string cProtocolParser::number()
{
	std::string str = "";
	str = m_scan.GetTokenStringQ(0);
	Match(NUM);
	return str;
}

int cProtocolParser::num()
{
	int n = atoi(m_scan.GetTokenStringQ(0));
	Match(NUM);
	return n;
}

std::string cProtocolParser::id()
{
	std::string str = m_scan.GetTokenStringQ(0);
	Match( ID );
	return str;
}

bool cProtocolParser::Match(eTokentype t )
{
	if (m_token == t)
	{
		m_token = m_scan.GetToken();
	}
	else
	{
		SyntaxError( "unexpected token -> " );
		PrintToken(m_token, m_scan.GetTokenStringQ(0) );
		std::cout << "\n";
	}
	return true;
}


void cProtocolParser::SyntaxError( const char *szMsg, ... )
{
	m_isError = true;
	char buf[256];
	va_list marker;
	va_start(marker, szMsg);
	vsprintf_s(buf, sizeof(buf), szMsg, marker);
	va_end(marker);
	std::cout << ">>>";
	std::cout << "Syntax error at line " << m_fileName << " " << m_scan.GetLineNo()
		<< ": " << buf;
}

