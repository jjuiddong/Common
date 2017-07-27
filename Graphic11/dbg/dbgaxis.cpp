
#include "stdafx.h"
#include "dbgaxis.h"

using namespace graphic;


cDbgAxis::cDbgAxis()
{
}

cDbgAxis::~cDbgAxis()
{
}


bool cDbgAxis::Create(cRenderer &renderer)
{
	const cColor colors[] = {cColor(1.f, 0.f, 0.f), cColor(0.f, 1.f, 0.f), cColor(0.f, 0.f, 1.f)};

	for (int i = 0; i < 3; ++i)
		m_lines[i].SetLine(renderer, Vector3(-1, -1, -1), Vector3(1, 1, 1), 1, colors[i].m_color);

	return true;
}


void cDbgAxis::SetAxis(const cBoundingBox &bbox)
{
	const Vector3 dim = bbox.GetDimension();
	const float size = max(dim.x, max(dim.y, dim.z)) * 0.005f;
	SetAxis(min(1,size), bbox);
}


void cDbgAxis::SetAxis(const float size, const cBoundingBox &bbox)
{
	const Vector3 dim = bbox.GetDimension();
	const Vector3 axis[] = { Vector3(dim.x*0.6f,0,0), Vector3(0,dim.y*1.2f,0), Vector3(0,0,dim.z*0.6f) };

	const Vector3 orig = Vector3(0, 0, 0) * bbox.m_tm;
	for (int i = 0; i < 3; ++i)
	{
		Vector3 v = axis[i] * bbox.m_tm;
		m_lines[i].SetLine(orig, v, size);
	}
}


void cDbgAxis::Render(cRenderer &renderer, const Matrix44 &tm)
{
	for (int i = 0; i < 3; ++i)
		m_lines[i].Render(renderer, tm);
}
