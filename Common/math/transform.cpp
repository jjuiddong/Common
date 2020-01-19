
#include "stdafx.h"
#include "transform.h"


using namespace common;
const Transform Transform::Identity;


Transform::Transform()
: pos(0,0,0)
, scale(1,1,1)
{
}

Transform::Transform(const Vector3 &pos_)
	: pos(pos_)
	, scale(1, 1, 1)
{
}

Transform::Transform(const Vector3 &pos_, const Vector3 &scale_)
	: pos(pos_)
	, scale(scale_)
{
}

Transform::Transform(const Vector3 &pos_, const Quaternion &rot_)
	: pos(pos_)
	, scale(Vector3(1,1,1))
	, rot(rot_)
{
}

Transform::Transform(const Vector3 &pos_, const Vector3 &scale_, const Quaternion &rot_)
	: pos(pos_)
	, scale(scale_)
	, rot(rot_)
{
}


// Scale * Rotation * Translation
Matrix44 Transform::GetMatrix() const
{
	Matrix44 S;
	S.SetScale(scale);

	Matrix44 tm;
	tm = S * rot.GetMatrix();

	tm._41 = pos.x;
	tm._42 = pos.y;
	tm._43 = pos.z;

	return tm;
}


Transform Transform::Inverse() const
{
	Transform v;
	v.pos = -pos;
	v.scale = Vector3(1/scale.x, 1/scale.y, 1/scale.z);
	v.rot = rot.Inverse();
	return v;
}


// Scale * Rotation * Translation
Transform Transform::operator*(const Transform &rhs) const
{
	const Matrix44 tm1 = rhs.rot.GetMatrix();

	Transform v;
	v.pos = ((pos * rhs.scale) * tm1) + rhs.pos;
	v.scale = scale * rhs.scale;
	v.rot = rot * rhs.rot;
	return v;

	//Transform v;
	//v.pos = pos + rhs.pos;
	//v.scale = Vector3(scale.x * rhs.scale.x
	//	, scale.y * rhs.scale.y
	//	, scale.z * rhs.scale.z);
	//v.rot = rot * rhs.rot;
	//return v;
}


const Transform& Transform::operator*=(const Transform &rhs)
{
	*this = operator*(rhs);
	return *this;
}
