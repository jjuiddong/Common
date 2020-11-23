
#include "stdafx.h"
#include "skeleton.h"

using namespace graphic;

cSkeleton::cSkeleton()
{
}

cSkeleton::~cSkeleton()
{
}

bool cSkeleton::Create(sRawMeshGroup2 *meshes)
{
	RETV(!meshes, false);

	m_tmOffset.resize(meshes->bones.size(), Matrix44::Identity);
	m_tmAni.resize(meshes->bones.size(), Matrix44::Identity);
	m_tmPose.resize(meshes->bones.size(), Matrix44::Identity);

	m_bones = meshes->bones; // Already Orderred
	SetInitializePose();

	return true;
}


void cSkeleton::UpdateHierarchyTransform()
{
	for (auto &bone : m_bones)
	{
		Matrix44 tm = m_tmAni[bone.id];
		if (bone.parentId >= 0)
			tm *= m_tmPose[bone.parentId];
		m_tmPose[bone.id] = tm;
	}	
}


void cSkeleton::SetInitializePose()
{
	for (uint i = 0; i < m_bones.size(); ++i)
	{
		m_tmAni[i] = m_bones[i].localTm;
		m_tmOffset[i] = m_bones[i].offsetTm;
	}
}
