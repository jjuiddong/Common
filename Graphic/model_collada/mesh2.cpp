
#include "stdafx.h"
#include "mesh2.h"
#include "bone2.h"

using namespace graphic;


cMesh2::cMesh2()
	: m_buffers(NULL)
	, m_skinnedBuffers(NULL)
	, m_tmPalette(NULL)
{
}

cMesh2::~cMesh2()
{
	Clear();
}


// Create Mesh
bool cMesh2::Create(cRenderer &renderer, const sRawMesh2 mesh, vector<Matrix44> *tmPalette)
{
	Clear();

	m_tmPalette = tmPalette;

	CreateMaterials(renderer, mesh);
	
	m_buffers = new cMeshBuffer(renderer, mesh);

	// Create Bone
	m_bones.reserve(mesh.bones.size());
	for (auto &bone : mesh.bones)
	{
		cBoneNode2 *p = new cBoneNode2();
		p->Create(bone);
		m_bones.push_back(p);
	}

	// Create Skinned Mesh
	if (m_buffers->m_isSkinned && !m_bones.empty())
		m_skinnedBuffers = new cMeshBuffer(renderer, mesh);

	return true;
}


void cMesh2::CreateMaterials(cRenderer &renderer, const sRawMesh2 &rawMesh)
{
	m_mtrls.push_back(cMaterial());
	m_mtrls.back().Init(rawMesh.mtrl);

	if (!rawMesh.mtrl.texture.empty())
		m_colorMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.texture));

	if (!rawMesh.mtrl.bumpMap.empty())
		m_normalMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.bumpMap));

	if (!rawMesh.mtrl.specularMap.empty())
		m_specularMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.specularMap));

	if (!rawMesh.mtrl.selfIllumMap.empty())
		m_selfIllumMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.selfIllumMap));
}


void cMesh2::Render(cRenderer &renderer, const Matrix44 &tm)
 // tm=Matrix44::Identity
{
	RET(!m_buffers);

	//const Matrix44 m = m_localTM * m_aniTM * m_TM * tm;
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&tm);

	if (!m_mtrls.empty())
		m_mtrls[0].Bind(renderer);
	if (!m_colorMap.empty())
		m_colorMap[0]->Bind(renderer, 0);

	if (m_skinnedBuffers)
	{
		m_skinnedBuffers->Bind(renderer);
		m_skinnedBuffers->Render(renderer);
	}
	else
	{
		m_buffers->Bind(renderer);
		m_buffers->Render(renderer);
	}
}


// Skinning Animation
bool cMesh2::Update(const float deltaSeconds)
{
	RETV(!m_buffers, false);

	if (m_skinnedBuffers)
	{
		cVertexBuffer &srcVtxBuffer = m_buffers->GetVertexBuffer();
		cVertexBuffer &dstVtxBuffer = m_skinnedBuffers->GetVertexBuffer();
		const int pos_offset = srcVtxBuffer.GetOffset(D3DDECLUSAGE_POSITION);
		const int normal_offset = srcVtxBuffer.GetOffset(D3DDECLUSAGE_NORMAL);
		const int blendWeight_offset = srcVtxBuffer.GetOffset(D3DDECLUSAGE_TEXCOORD, 1);
		const int blendIndices_offset = srcVtxBuffer.GetOffset(D3DDECLUSAGE_TEXCOORD, 2);
		const int stride = srcVtxBuffer.GetSizeOfVertex();

		BYTE *src = (BYTE*)srcVtxBuffer.Lock();
		BYTE *dst = (BYTE*)dstVtxBuffer.Lock();
		RETV(!src, false);

		for (int i = 0; i < srcVtxBuffer.GetVertexCount(); ++i)
		{
			Vector3 &p = *(Vector3*)(src + (i*stride) + pos_offset);
			Vector3 &p_ = *(Vector3*)(dst + (i*stride) + pos_offset);
			Vector3 &n = *(Vector3*)(src + (i*stride) + normal_offset);
			Vector3 &n_ = *(Vector3*)(dst + (i*stride) + normal_offset);
			float *weight = (float*)(src + (i*stride) + blendWeight_offset);
			float *boneIndices = (float*)(src + (i*stride) + blendIndices_offset);

			const Matrix44 &m0 = (*m_tmPalette)[(int)boneIndices[0]];
			const Matrix44 &m1 = (*m_tmPalette)[(int)boneIndices[1]];
			const Matrix44 &m2 = (*m_tmPalette)[(int)boneIndices[2]];
			const Matrix44 &m3 = (*m_tmPalette)[(int)boneIndices[3]];

			const Vector3 p1 = (p * m0) * weight[0];
			const Vector3 p2 = (p * m1) * weight[1];
			const Vector3 p3 = (p * m2) * weight[2];
			const Vector3 p4 = (p * m3) * weight[3];

			const Vector3 n1 = n.MultiplyNormal(m0) * weight[0];
			const Vector3 n2 = n.MultiplyNormal(m1) * weight[1];
			const Vector3 n3 = n.MultiplyNormal(m2) * weight[2];
			const Vector3 n4 = n.MultiplyNormal(m3) * weight[3];

			p_ = p1 + p2 + p3 + p4;
			n_ = n1 + n2 + n3 + n4;
		}		

		srcVtxBuffer.Unlock();
		dstVtxBuffer.Unlock();
	}

	return true;
}


void cMesh2::Clear()
{
	SAFE_DELETE(m_buffers);
	SAFE_DELETE(m_skinnedBuffers);

	m_mtrls.clear();
	m_colorMap.clear();
	m_normalMap.clear();
	m_specularMap.clear();
	m_selfIllumMap.clear();

	for (auto &p : m_bones)
		delete p;
	m_bones.clear();
}
