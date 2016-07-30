
#include "stdafx.h"
#include "circleline.h"

using namespace graphic;

cCircleLine::cCircleLine() 
	: m_radius(0)
{
}

cCircleLine::~cCircleLine()
{
}


bool cCircleLine::Create(cRenderer &renderer, const Vector3 &center, const float radius, const float width, 
	const float start, const float last)
{
	m_radius = radius;

	Vector3 p0 = center + Vector3(cosf(start) * radius, sinf(start) * radius, 0);

	const int slice = 20;
	//const float inc = MATH_PI * 2.f / (float)(slice - 1);
	const float inc = (last - start) / (float)slice;
	for (int i = 0; i < slice; ++i)
	{
		const float angle = inc*(i+1) + start;
		Vector3 p1(cosf(angle) * radius, sinf(angle) * radius, 0);

		m_lines[i].SetLine(renderer, p0, p1, width);
		m_lines[i].GetMaterial().InitBlack();
		p0 = p1;
	}

	return true;
}


void cCircleLine::Render(cRenderer &renderer)
{
	for (int i = 0; i < 20; ++i)
		m_lines[i].Render(renderer);
}


void cCircleLine::MuliplyTransform(const Matrix44 &tm)
{
	for (int i = 0; i < 20; ++i)
	{
		m_lines[i].SetTransform( m_lines[i].GetTransform() * tm );
	}
}