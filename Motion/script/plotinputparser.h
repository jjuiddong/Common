//
// Plot 입력으로 들어갈 스트링을 생성하는 명령어를 분석하는 클래스.
// 변수는 접두사가 $,@ 인 것만 인식한다.
//
#pragma once

#include "plotinputscript.h"


class cPlotInputParser
{
public:
	cPlotInputParser();
	virtual ~cPlotInputParser();

	bool Read(const string &fileName);
	bool ParseStr(const string &source);
	string Execute();
	int ExecuteBinary(BYTE *buffer, const int bufferLen);


protected:
	plotinputscript::sStatement *m_stmt;

	
	//BNF
	// {[symbol] + [string]}
	plotinputscript::sStatement* statement(string &src);
	string symbol(string &src);
	string str(string &src);
};


void ParsePlotInputStringFormat(const string &str, OUT vector<cPlotInputParser> &out);
