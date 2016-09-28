
#include "stdafx.h"
#include "mathinterpreter.h"

using namespace mathscript;


cMathInterpreter::cMathInterpreter()
{
}

cMathInterpreter::~cMathInterpreter()
{
}


float cMathInterpreter::Excute(const sStatement *p)
{
	RETV(!p, 0);
	
	float val = 0;
	if (p->assign)
		val = assignstmt(p->assign);
	else if (p->expr)
		val = expr(p->expr);

	Excute(p->next);
	
	return val;
}


float cMathInterpreter::assignstmt(const sAssignStmt *p)
{
	RETV(!p, 0);

	const float val = expr(p->expr);
	script::g_symbols[p->id].fVal = val;
	script::g_symbols[p->id].type = script::FIELD_TYPE::T_FLOAT;

	return val;
}


float cMathInterpreter::expr(const sExpr*p)
{
	RETV(!p, 0);
	float val = term(p->term1);

	if (p->expr)
	{
		switch (p->op)
		{
		case OP_PLUS::PLUS:
			val += expr(p->expr);
			break;
		case OP_PLUS::MINUS:
			val -= expr(p->expr);
			break;
		}
	}

	return val;
}


float cMathInterpreter::term(const sTerm *p)
{
	RETV(!p, 0);
	float val = factor(p->factor1);

	if (p->term)
	{
		switch (p->op)
		{
		case OP_MULTI::MULTI:
			val *= term(p->term);
			break;
		case OP_MULTI::DIVIDE:
			val /= term(p->term);
			break;
		}
	}

	return val;
}


float cMathInterpreter::func(const sFunc *p)
{
	using namespace script;

	RETV(!p, 0);
	float val = 0;

	if (p->id == "abs") // abs( val )
	{
		if (p->exprs)
		{
			val = abs( expr(p->exprs->expr) );
		}
	}
	else if (p->id == "floor") // floor( val )
	{
		if (p->exprs)
		{
			val = floor(expr(p->exprs->expr));
		}
	}
	else if (p->id == "round") // round( val )
	{
		if (p->exprs)
		{
			val = round(expr(p->exprs->expr));
		}
	}
	else if (p->id == "angle2rad") // angle2rad( val )
	{
		if (p->exprs)
		{
			val = ANGLE2RAD(expr(p->exprs->expr));
		}
	}
	else if (p->id == "rad2angle") // rad2angle( val )
	{
		if (p->exprs)
		{
			val = RAD2ANGLE(expr(p->exprs->expr));
		}
	}
	else if (p->id == "atan") // atan( y, x )
	{
		if (p->exprs && p->exprs->expr && p->exprs->next->expr) // null check
		{
			val = atan2( expr(p->exprs->expr), expr(p->exprs->next->expr) );
		}
	}
	else if (p->id == "clamp") // script : clamp(value, min, max)
	{
		if (p->exprs && p->exprs->expr && p->exprs->next->expr && p->exprs->next->next && p->exprs->next->next->expr) // null check
		{
			// common::clamp(min, max, val)
			val = common::clamp(expr(p->exprs->next->expr), expr(p->exprs->next->next->expr), expr(p->exprs->expr));
		}
	}
	else if (p->id == "scale") // script : scale(value, center, rate)
	{
		if (p->exprs && p->exprs->expr && p->exprs->next->expr && p->exprs->next->next && p->exprs->next->next->expr) // null check
		{
			// common::centerRate(rate, center, val)
			val = common::centerRate(expr(p->exprs->next->next->expr), expr(p->exprs->next->expr), expr(p->exprs->expr));
		}
	}


	return val;
}


float cMathInterpreter::factor(const sFactor *p)
{
	using namespace script;

	RETV(!p, 0);
	float val = 0;

	switch (p->type)
	{
	case FACTOR_TYPE::ID:
		switch (g_symbols[p->id].type)
		{
		case FIELD_TYPE::T_BOOL: val = (float)(script::g_symbols[p->id].bVal? 1 : 0); break;
		case FIELD_TYPE::T_UINT: val = (float)script::g_symbols[p->id].uVal; break;
		case FIELD_TYPE::T_SHORT: val = (float)script::g_symbols[p->id].sVal; break;
		case FIELD_TYPE::T_INT: val = (float)script::g_symbols[p->id].iVal; break;
		case FIELD_TYPE::T_FLOAT: val = script::g_symbols[p->id].fVal; break;
		case FIELD_TYPE::T_DOUBLE: val = (float)script::g_symbols[p->id].dVal; break;
		default:
			break;
		}
		break;
	case FACTOR_TYPE::NUMBER:
		val = p->number;
		break;
	case FACTOR_TYPE::EXPR:
		val = expr(p->expr);
		break;
	case FACTOR_TYPE::FUNC:
		val = func(p->func);
		break;
	}

	return val;
}
