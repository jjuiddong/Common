
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
 	float val = dterm(p->dterm);

	if (p->term)
	{
		if (p->op == OP_MULTI::MULTI)
			val *= term(p->term);
	}

	return val;
}


float cMathInterpreter::dterm(const sDTerm *p)
{
	RETV(!p, 0);

	float val = factor(p->factor1);

	if (p->factor2)
	{
		if (p->op == OP_MULTI::DIVIDE)
			val /= factor(p->factor2);
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


float cMathInterpreter::func(const sFunc *p)
{
	using namespace script;

	RETV(!p, 0);
	float val = 0;

	if (p->id == "abs") // abs( val )
	{
		if (p->exprs)
		{
			val = abs(expr(p->exprs->expr));
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
			val = atan2(expr(p->exprs->expr), expr(p->exprs->next->expr));
		}
	}
	else if (p->id == "clamp") // script : clamp(value, min, max)
	{
		if (p->exprs && p->exprs->expr && p->exprs->next->expr && p->exprs->next->next && p->exprs->next->next->expr) // null check
		{
			val = common::clamp(expr(p->exprs->next->expr), expr(p->exprs->next->next->expr), expr(p->exprs->expr));
		}
	}
	else if (p->id == "scale") // script : scale(value, center, rate)
	{
		if (p->exprs && p->exprs->expr && p->exprs->next->expr && p->exprs->next->next && p->exprs->next->next->expr) // null check
		{
			val = common::centerRate(expr(p->exprs->next->next->expr), expr(p->exprs->next->expr), expr(p->exprs->expr));
		}
	}
	else if (p->id == "quat2roll") // script : quat2roll(x,y,z,w)
	{
		if (p->exprs && p->exprs->expr && p->exprs->next->expr && p->exprs->next->next && p->exprs->next->next->expr && p->exprs->next->next->next && p->exprs->next->next->next->expr) // null check
		{
			Quaternion q(expr(p->exprs->expr), expr(p->exprs->next->expr), expr(p->exprs->next->next->expr), expr(p->exprs->next->next->next->expr));

			const Matrix44 tm = q.GetMatrix();
			Vector3 front = Vector3(1, 0, 0) * tm;
			Vector3 right = Vector3(0, 0, -1) * tm;
			Vector3 horz = Vector3(0, 1, 0).CrossProduct(front);
			Vector3 back = right.CrossProduct(horz);
			right.Normalize();
			front.Normalize();
			horz.Normalize();
			back.Normalize();
			val = (float)acos(clamp(-1, 1, right.DotProduct(horz)));
			if (front.DotProduct(back) > 0)
				val = -val;
		}
	}
	else if (p->id == "quat2pitch") // script : quat2pitch(x,y,z,w)
	{
		if (p->exprs && p->exprs->expr && p->exprs->next->expr && p->exprs->next->next && p->exprs->next->next->expr && p->exprs->next->next->next && p->exprs->next->next->next->expr) // null check
		{
			Quaternion q(expr(p->exprs->expr), expr(p->exprs->next->expr), expr(p->exprs->next->next->expr), expr(p->exprs->next->next->next->expr));

			const Matrix44 tm = q.GetMatrix();
			Vector3 front = Vector3(1, 0, 0) * tm;
			Vector3 right = Vector3(0, 0, -1) * tm;
			Vector3 horz = -Vector3(0, 1, 0).CrossProduct(right);
			Vector3 right2 = front.CrossProduct(horz);
			right.Normalize();
			front.Normalize();
			horz.Normalize();
			right2.Normalize();
			val = (float)acos(clamp(-1, 1, front.DotProduct(horz)));
			if (right.DotProduct(right2) < 0)
				val = -val;
		}
	}
	else if (p->id == "quat2yaw") // script : quat2yaw(x,y,z,w)
	{
		if (p->exprs && p->exprs->expr && p->exprs->next->expr && p->exprs->next->next && p->exprs->next->next->expr && p->exprs->next->next->next && p->exprs->next->next->next->expr) // null check
		{
			Quaternion q(expr(p->exprs->expr), expr(p->exprs->next->expr), expr(p->exprs->next->next->expr), expr(p->exprs->next->next->next->expr));

			const Matrix44 tm = q.GetMatrix();
			Vector3 left = Vector3(0, 0, 1) * tm;
			Vector3 front = Vector3(0, 1, 0).CrossProduct(left);
			Vector3 up = front.CrossProduct(Vector3(1, 0, 0));
			left.Normalize();
			front.Normalize();
			up.Normalize();
			val = (float)acos(clamp(-1, 1, front.DotProduct(Vector3(1, 0, 0))));
			if (up.DotProduct(Vector3(0, 1, 0)) < 0)
				val = -val;
		}
	}

	return val;
}
