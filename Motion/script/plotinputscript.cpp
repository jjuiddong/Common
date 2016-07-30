
#include "stdafx.h"
#include "plotinputscript.h"


using namespace plotinputscript;


void plotinputscript::rm_statement(sStatement *p)
{
	RET(!p);
	rm_statement(p->next);
	delete p;
}
