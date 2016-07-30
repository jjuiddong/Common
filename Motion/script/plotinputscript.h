//
// Plot 입력으로 들어갈 스트링을 만드는 명령어를 분석할 때 필요한, 함수 와 구조체를 선언한다.
//
#pragma once

#include "protocolparser.h"


namespace plotinputscript
{

	struct sStatement
	{
		string symbol;
		string str;
		sStatement *next;
	};
	
	
	void rm_statement(sStatement *p);
}
