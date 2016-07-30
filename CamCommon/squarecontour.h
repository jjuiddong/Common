//
// 사각형 내의 점 위치를 uv 값으로 계산한다.
//
//
#pragma once

#include "rectcontour.h"


class cSquareContour : public cRectContour
{
public:
	cSquareContour();
	virtual ~cSquareContour();

	void InitSquare();
	bool GetUV(const cv::Point &pos, OUT cv::Point2f &uv);
	cSquareContour& operator = (const cRectContour &rhs);
	cSquareContour& operator = (const cSquareContour &rhs);


	Vector3 m_v[4];
};

