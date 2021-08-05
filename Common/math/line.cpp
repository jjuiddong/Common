
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


// return distance of line to 3d pos
float Line::GetDistance(const Vector3 &position) const
{
	const Vector3 toPoint = position - pos;
	float distOnLine = toPoint.DotProduct(dir) / len;
	distOnLine = clamp(-1.f, 1.f, distOnLine) * len;
	const Vector3 pointOnLine = pos + dir * distOnLine;
	const float dist = pointOnLine.Distance(position);
	return dist;
}


// return distance of line to line
// reference: https://wickedengine.net/2020/04/26/capsule-collision-detection/
float Line::GetDistance(const Line &line) const
{
	const Vector3 a0 = dir * len + pos;
	const Vector3 a1 = dir * -len + pos;
	const Vector3 b0 = line.dir * line.len + line.pos;
	const Vector3 b1 = line.dir * -line.len + line.pos;

	const Vector3 v0 = b0 - a0;
	const Vector3 v1 = b1 - a0;
	const Vector3 v2 = b0 - a1;
	const Vector3 v3 = b1 - a1;

	// squared distance
	const float d0 = v0.DotProduct(v0);
	const float d1 = v0.DotProduct(v1);
	const float d2 = v0.DotProduct(v2);
	const float d3 = v0.DotProduct(v3);

	// select best potential endpoint on line A:
	Vector3 bestA;
	if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
		bestA = a1;
	else
		bestA = a0;

	// select point on line B line segment nearest to best potential endpoint on A line:
	Vector3 bestB = line.GetPointOnLine(bestA);
	// now do the same for line A segment:
	bestA = GetPointOnLine(bestB);
	return bestA.Distance(bestB);
}


// project line and 3d pos
Vector3 Line::Projection(const Vector3 &position) const
{
	const float l = dir.DotProduct(position - pos);
	return dir * l + pos;
}


// get point one line
Vector3 Line::GetPointOnLine(const Vector3 &position) const
{
	const Vector3 toPoint = position - pos;
	float distOnLine = toPoint.DotProduct(dir) / len;
	distOnLine = clamp(-1.f, 1.f, distOnLine) * len;
	return dir * distOnLine + pos;
}
