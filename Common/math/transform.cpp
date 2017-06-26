
#include "stdafx.h"
#include "transform.h"


using namespace common;


const Transform Transform::Identity;


Transform::Transform()
: pos(0,0,0)
, scale(1,1,1)
, rot(0,0,0,1)
{

}


// Scale * Rotation * Traslation
Matrix44 Transform::GetMatrix()
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
