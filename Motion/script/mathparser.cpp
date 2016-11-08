
#include "stdafx.h"
#include "mathparser.h"
#include "scriptutil.h"

using namespace mathscript;
using namespace script;


cMathParser::cMathParser() : 
m_stmt(NULL)
{
}

cMathParser::~cMathParser()
{
	if (m_stmt)
	{
		rm_statement(m_stmt);
		m_stmt = NULL;
	}
}


// read script file
bool cMathParser::Read(const string &fileName)
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


// parse script
bool cMathParser::ParseStr(const string &source)
{
	string src = source;
	trim(src);
	if (src.empty())
		return true;

	// debug log
	if (script::g_isLog)
	{
		dbg::Log("math parser input string \n");
		dbg::Log("\t%s \n\n", source.c_str());
	}

	rm_statement(m_stmt);
	m_stmt = statement(src);

	return (m_stmt != NULL);
}


//statement -> {assign_stmt}
sStatement* cMathParser::statement(string &src)
{
	trim(src);

	if (src.empty())
		return NULL;

	sStatement *stmt = new sStatement({});
	stmt->assign = assign_stmt(src);
	if (!stmt->assign)
		stmt->expr = expression(src);

	stmt->next = statement(src);
	return stmt;
}


// assign_stmt -> id = exp
sAssignStmt* cMathParser::assign_stmt(string &src)
{
	trim(src);

	sAssignStmt *assign = new sAssignStmt({});
	const int idx = src.find_first_of('=');
	if (idx == string::npos)
	{
		delete assign;
		return NULL;
	}

	assign->id = id(src);
	trim(src);
	match(src, '=');
	trim(src);
	assign->expr = expression(src);
	return assign;
}


//- exp -> term +- expr | term
sExpr* cMathParser::expression(string &src)
{
	trim(src);

	sExpr *expr = new sExpr({});

	expr->term1 = term(src);
	trim(src);

	if (check(src, '+'))
	{
		match(src, '+');
		expr->op = OP_PLUS::PLUS;
		expr->expr = expression(src);
	}
	else if (check(src, '-'))
	{
		match(src, '-');
		expr->op = OP_PLUS::MINUS;
		expr->expr = expression(src);
	}

	return expr;
}


//- term -> dterm * term | dterm
sTerm* cMathParser::term(string &src)
{
	trim(src);

	sTerm *t = new sTerm({});
	t->dterm = dterm(src);
	trim(src);

	if (check(src, '*'))
	{
		match(src, '*');
		t->op = OP_MULTI::MULTI;
		t->term = term(src);
	}

	return t;
}


//- dterm -> factor / factor | factor
sDTerm* cMathParser::dterm(string &src)
{
	trim(src);

	sDTerm *t = new sDTerm({});
	t->factor1 = factor(src);
	t->factor2 = NULL;
	trim(src);

	if (check(src, '/'))
	{
		match(src, '/');
		t->op = OP_MULTI::DIVIDE;
		t->factor2 = factor(src);
	}

	return t;
}


//factor -> id, number, func, (exp)
sFactor* cMathParser::factor(string &src)
{
	trim(src);

	sFactor *factor = new sFactor({});

	bool checkNum = false;
	for (int i = 0; i < g_numLen; ++i)
	{
		if (compare(src, g_numStr[i]))
		{
			checkNum = true;
			break;
		}
	}

	if (checkNum)
	{
		factor->number = number(src);
		factor->type = FACTOR_TYPE::NUMBER;
	}
	else if (check(src, '('))
	{
		match(src, '(');
		factor->expr = expression(src);
		factor->type = FACTOR_TYPE::EXPR;
		match(src, ')');		
	}
	else
	{
		string strId = id(src);
		if (check(src, '('))
		{
			match(src, '(');
 			factor->func = func(strId, src);
 			factor->type = FACTOR_TYPE::FUNC;
		}
		else
		{
			factor->id = strId;
			factor->type = FACTOR_TYPE::ID;
		}
	}

	return factor;
}


// func -> id ( expr_list )
mathscript::sFunc* cMathParser::func(const string &name, string &src)
{
	sFunc *fn = new sFunc({});
	fn->id = name;
	fn->exprs = expression_list(src, true);
	match(src, ')');
	return fn;
}


//- expr_list -> expression {,expression}
mathscript::sExprList* cMathParser::expression_list(string &src, const bool isFirst)
{
	if (!isFirst)
	{
		if (!check(src, ','))
			return NULL;
		match(src, ',');
	}

	sExprList *exprs = new sExprList({});
	exprs->expr = expression(src);
	exprs->next = expression_list(src, false);
	return exprs;
}

