
#include "stdafx.h"
#include "circle.h"

using namespace graphic;

cCircle::cCircle()
	: cNode2(common::GenerateId(), "circle", eNodeType::MODEL)
	, m_radius(1)
{
}

cCircle::~cCircle()
{	
}


// 원 생성
bool cCircle::Create(cRenderer &renderer, const Vector3 &center, const float radius, const int slice
	, const cColor &color //= cColor::BLACK
)
{
	m_radius = radius;
	m_shape.Create(renderer, center, radius, slice, color);
	return true;
}


// 출력.
void cCircle::Render(cRenderer &renderer)
{
	m_shape.Render(renderer);
}
