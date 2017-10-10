
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

	for (u_int i = 0; i < meshes->bones.size(); ++i)
	{
		m_tmAni[i] = meshes->bones[i].localTm;
		m_tmOffset[i] = meshes->bones[i].offsetTm;
	}

	// make bone ordered
	//{
	//	m_bones.reserve(meshes->bones.size());

	//	set<int> testBoneId;
	//	for (auto &bone : meshes->bones)
	//	{
	//		int tmp[64]; // max hierarchy 64
	//		int size = 0;
	//		int parentId = bone.parentId;
	//		while (parentId >= 0)
	//		{
	//			auto it = testBoneId.find(parentId);
	//			if (testBoneId.end() != it)
	//				break; // already exist, break

	//			tmp[size++] = parentId;
	//			parentId = m_meshes->bones[parentId].parentId;
	//		}

	//		// reverse push (insert parent to child)
	//		for (int i = size - 1; i >= 0; --i)
	//		{
	//			testBoneId.insert(tmp[i]);
	//			m_bones.push_back(m_meshes->bones[tmp[i]]);
	//		}

	//		testBoneId.insert(bone.id);
	//		m_bones.push_back(bone);
	//	}
	//}

	m_bones = meshes->bones; // Already Orderred

	return true;
}


void cSkeleton::UpdateHierarcyTransform()
{
	for (auto &bone : m_bones)
	{
		Matrix44 tm = m_tmAni[bone.id];
		int parentId = bone.parentId;
		if (parentId >= 0)
			tm *= m_tmPose[parentId];
		m_tmPose[bone.id] = tm;
	}
}

