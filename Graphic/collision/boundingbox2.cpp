
#include "stdafx.h"
#include "boundingbox2.h"

using namespace graphic;


cBoundingBox2::cBoundingBox2()
{
}


void cBoundingBox2::SetLineBoundingBox(const Vector3 &p0, const Vector3 &p1, const float width)
{
	Vector3 v = p1 - p0;
	const float len = v.Length();
	v.Normalize();

	Matrix44 matS;
	matS.SetScale(Vector3(width, width, len / 2.f));

	Matrix44 matT;
	matT.SetTranslate((p0 + p1) / 2.f);

	Quaternion q(Vector3(0, 0, 1), v);
	m_tm = matS * q.GetMatrix() * matT;
}
