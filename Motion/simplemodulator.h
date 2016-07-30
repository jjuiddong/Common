//
// pid, defference, scaling 계산을 하는 클래스
//
// 스크립트로부터 파라메터 값을 받아온 후, 계산한다.
//
#pragma once

#include "modulator.h"


class cSimpleModulator : public common::cConfig, public cModulator
{
public:
	cSimpleModulator();
	virtual ~cSimpleModulator();


protected:
	virtual void InitDefault() override;
	virtual void UpdateParseData() override;
};
