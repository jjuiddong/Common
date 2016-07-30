
#include "stdafx.h"
#include "motionscript.h"

using namespace motion;


void motion::rm_program(sProgram*p)
{
	RET(!p);
	rm_command(p->cmd);
	rm_componentlist(p->compList);
	delete p;
}

void motion::rm_command(sCommand *p)
{
	RET(!p);
	delete p;
}

void motion::rm_componentlist(sComponentList *p)
{
	RET(!p);
	rm_component(p->comp);
	rm_componentlist(p->next);
	delete p;
}

void motion::rm_component(sComponent *p)
{
	RET(!p);
	rm_componentheader(p->header);
	rm_command(p->cmd);
	rm_subcomponentlist(p->subCompList);
	delete p;
}

void motion::rm_componentheader(sComponentHeader *p)
{
	RET(!p);
	delete p;
}

void motion::rm_subcomponentlist(sSubComponentList *p)
{
	RET(!p);
	rm_subcomponent(p->subComp);
	rm_subcomponentlist(p->next);
	delete p;
}

void motion::rm_subcomponent(sSubComponent *p)
{
	RET(!p);
	rm_componentheader(p->header);
	delete p;
}
