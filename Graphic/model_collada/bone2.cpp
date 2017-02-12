
#include "stdafx.h"
#include "bone2.h"

using namespace graphic;

cBoneNode2::cBoneNode2()
	: m_id(0)
{
}

cBoneNode2::~cBoneNode2()
{
}


bool cBoneNode2::Create(const sRawBone &bone)
{
	m_id = bone.id;
	m_name = bone.name;
	m_localTm = bone.localTm;
	m_localTm.Transpose();

	return true;
}
