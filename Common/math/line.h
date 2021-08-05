//
// 2019-05-08, jjuiddong
// 3d line
//
#pragma once


namespace common
{
	struct Line
	{
		Line();
		Line(const Vector3 &dir0, const Vector3 &pos0, const float len0);
		Line(const Vector3 &p0, const Vector3 &p1);

		bool Create(const Vector3 &p0, const Vector3 &p1);
		float GetDistance(const Vector3 &position) const;
		float GetDistance(const Line &line) const;
		Vector3 Projection(const Vector3 &position) const;
		Vector3 GetPointOnLine(const Vector3 &position) const;

		Vector3 dir; // unit vector
		Vector3 pos; // line center
		float len; // line half length
	};
}
