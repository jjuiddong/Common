
#include "stdafx.h"
#include "squarecontour.h"

using namespace cv;


cSquareContour::cSquareContour()
{

}
cSquareContour::~cSquareContour()
{

}


// 꼭지점 영역의 벡터를 구한다.
//
//         <- v[3]
//  -----------P1
// |           | v[2]
// |  |        | |
// |v[0]       |
// P0 ----------
//  v[1] ->
//
//
void cSquareContour::InitSquare()
{
	// cRectContour 의 init가 호출된 상태여야 한다.
	 
	// P0 = m_contours[3]
	// P1 = m_contours[1]
	Vector3 v[4];
	v[0] = Vector3((float)m_contours[0].x, (float)m_contours[0].y, 0);
	v[1] = Vector3((float)m_contours[1].x, (float)m_contours[1].y, 0);
	v[2] = Vector3((float)m_contours[2].x, (float)m_contours[2].y, 0);
	v[3] = Vector3((float)m_contours[3].x, (float)m_contours[3].y, 0);

	m_v[0] = v[0] - v[3];
	m_v[1] = v[2] - v[3];
	m_v[2] = v[0] - v[1];
	m_v[3] = v[2] - v[1];
	
// 	for (int i = 0; i < 4; ++i)
// 		m_v[i].Normalize();
}


//
// 두 개의 삼각형으로 분리해서 계산한다.
//
bool cSquareContour::GetUV(const Point &pos, OUT Point2f &uv)
{
	const float u1 = ((float)(pos.x - m_contours[3].x)) / (m_v[0].x + m_v[1].x);
	const float v1 = ((float)(pos.y - m_contours[3].y)) / (m_v[0].y + m_v[1].y);

	const float u2 = ((float)(pos.x - m_contours[1].x)) / (m_v[2].x + m_v[3].x);
	const float v2 = ((float)(pos.y - m_contours[1].y)) / (m_v[2].y + m_v[3].y);
	
	if ((u1*v1 < 0) || (u2*v2 < 0))
		return false;

	if (((u1 + v1) > 1.f))
	{
		// 윗 삼각형
		uv.x = 1.f - u2;
		uv.y = 1.f - v2;
	}
	else
	{
		// 아래 삼각형
		uv.x = u1;
		uv.y = v1;
	}

	std::cout << "u1 = " << u1  << " v1 = " << v1 << " u2 = " << u2 << " v2 = " << v2 << std::endl;
	//std::cout << "u = " << u1 + u2 << " v = " << v1 + v2 << " uv1 = " << u1 + v1 << " uv2 = " << u2+v2 << std::endl;

	return true;
}


cSquareContour& cSquareContour::operator = (const cRectContour &rhs)
{
	if (this != &rhs)
	{
		cRectContour::operator=(rhs);
		InitSquare();
	}
	return *this;
}


cSquareContour& cSquareContour::operator = (const cSquareContour &rhs)
{
	if (this != &rhs)
	{
		cRectContour::operator=(rhs);
		InitSquare();
	}
	return *this;
}

