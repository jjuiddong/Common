//
// 연속된 메모리를 정의하는 스크립트를 분석한다.
// 구조체 선언을 스크립트로 처리할 수 있게 했다.
//
// exmaple
// 4, float
// 4, float
// 4, float
// 4, float
// 4, float
//
//
#pragma once

#include "scriptutil.h"


struct sUDPField
{
	int bytes; // 차지하는 메모리 바이트 크기
	script::FIELD_TYPE::TYPE type; // 데이터 타입
};


class cProtocolParser
{
public:
	cProtocolParser();
	virtual ~cProtocolParser();

	bool Read(const string &fileName, const string &delimeter);
	bool ParseStr(const string &source, const string &delimeter= "\r\n");

	vector<sUDPField> m_fields;	
	int m_fieldsByteSize;
};
