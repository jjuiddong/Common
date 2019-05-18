
#include "stdafx.h"
#include "line.h"

using namespace common;


Line::Line()
	: len(0)
{
}

Line::Line(const Vector3 &dir0, const Vector3 &pos0, const float len0)
	: dir(dir0)
	, pos(pos0)
	, len(len0)
{
}

Line::Line(const Vector3 &p0, const Vector3 &p1)
{
	Create(p0, p1);
}


bool Line::Create(const Vector3 &p0, const Vector3 &p1)
{
	pos = (p0 + p1) / 2.f;
	dir = (p1 - p0).Normal();
	len = (p1 - p0).Length() / 2.f;
	return true;
}


float Line::GetDistance(const Vector3 &position) const
{
	const Vector3 toPoint = position - pos;
	float distOnLine = toPoint.DotProduct(dir) / len;
	distOnLine = clamp(-1.f, 1.f, distOnLine) * len;
	const Vector3 pointOnLine = pos + dir * distOnLine;
	const float dist = pointOnLine.Distance(position);
	return dist;
}
