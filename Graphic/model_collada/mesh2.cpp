
#include "stdafx.h"
#include "mesh2.h"
#include "skeleton.h"

using namespace graphic;


cMesh2::cMesh2()
	: m_buffers(NULL)
	, m_skinnedBuffers(NULL)
	, m_skeleton(NULL)
{
}

cMesh2::~cMesh2()
{
	Clear();
}


// Create Mesh
bool cMesh2::Create(cRenderer &renderer, const sRawMesh2 mesh, cSkeleton *skeleton)
{
	Clear();

	m_name = mesh.name;
	m_skeleton = skeleton;
	m_bones = mesh.bones;
	m_localTm = mesh.localTm;

	CreateMaterials(renderer, mesh);
	
	m_buffers = new cMeshBuffer(renderer, mesh);

	// Create Skinned Mesh
	if (skeleton && m_buffers->m_isSkinned)
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

	const Matrix44 m = m_localTm * tm;
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&m);

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
		vector<Matrix44> &tmPalette = m_skeleton->m_tmPose;

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

			p_ = Vector3(0, 0, 0);
			n_ = Vector3(0, 0, 0);
			for (u_int k = 0; k < m_bones.size(); ++k)
			{
				const Matrix44 &m = tmPalette[m_bones[(int)boneIndices[k]].id];
				const Matrix44 &offset = m_bones[(int)boneIndices[k]].offsetTm;
				const Matrix44 tm = offset * m;
				const Vector3 p1 = (p * tm) * weight[k];
				const Vector3 n1 = n.MultiplyNormal(tm) * weight[k];
				p_ += p1;
				n_ += n1;
			}
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
}
