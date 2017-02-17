
#include "stdafx.h"
#include "skeleton.h"

using namespace graphic;

cSkeleton::cSkeleton()
{
}

cSkeleton::~cSkeleton()
{
}

bool cSkeleton::Create(const vector<sRawBone2> &bones)
{
	m_tmOffset.resize(bones.size(), Matrix44::Identity);
	m_tmAni.resize(bones.size(), Matrix44::Identity);
	m_tmPose.resize(bones.size(), Matrix44::Identity);
	m_bones2 = bones;

	for (u_int i = 0; i < bones.size(); ++i)
	{
		m_tmAni[i] = bones[i].localTm;
		m_tmOffset[i] = bones[i].offsetTm;
	}

	return true;
}


void cSkeleton::UpdateHierarcyTransform()
{
	for (auto &bone : m_bones2)
	{
		Matrix44 tm = m_tmAni[bone.id];
		int parentId = bone.parentId;
		while (parentId >= 0)
		{
			tm *= m_tmAni[parentId];
			parentId = m_bones2[parentId].parentId;
		}
		m_tmPose[bone.id] = tm;
	}	
}
