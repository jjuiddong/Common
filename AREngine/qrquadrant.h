//
// 2016-03-17, jjuiddong
//
// QRCode가 격자무늬로 나열 되어있을 때, 카메라 위치와 가장 가까운 4점을 찾는다.
//
//
#pragma once

#include "arutil.h"


namespace ar
{

	void Init();
	bool FindQuadrant(const int tableW, const int tableH, const int nearId, const ar::sQRPos *qrMap, OUT int *nearQuad);

}
