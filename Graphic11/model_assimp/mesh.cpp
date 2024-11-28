
#include "mesh.h"
#include "stdafx.h"
#include "skeleton.h"

using namespace graphic;


cMesh::cMesh()
	: m_buffers(NULL)
	, m_shader(NULL)
	, m_renderFlags(eRenderFlag::VISIBLE | eRenderFlag::NOALPHABLEND)
	, m_isBeginShader(true)
{
}

cMesh::~cMesh()
{
	Clear();
}


// Create Mesh
bool cMesh::Create(cRenderer &renderer, INOUT sRawMesh2 &mesh
	, const bool calculateTangentBinormal //= false
	, const StrPath &modelFileName //= ""
)
{
	Clear();

	m_name = mesh.name;
	m_renderFlags = mesh.renderFlags;
	m_bones = mesh.bones;

	CreateMaterials(renderer, mesh, modelFileName);
	
	m_buffers = new cMeshBuffer(renderer, mesh);

	return true;
}


void cMesh::CreateMaterials(cRenderer &renderer, const sRawMesh2 &rawMesh
	, const StrPath &modelFileName //= ""
)
{
	m_mtrls.push_back(cMaterial());
	m_mtrls.back().Init(rawMesh.mtrl);

	cResourceManager *rm = cResourceManager::Get();

	if (rawMesh.mtrl.texture.empty())
	{
		m_colorMap.push_back(
			rm->LoadTexture2(renderer, "", g_defaultTexture));
	}
	else
	{
		// 먼저, 모델파일이 있는 경로에서 먼저 텍스쳐를 찾는다.
		cTexture *tex = nullptr;
		if (modelFileName.empty())
		{
			// 모델파일명이 없다면 기본처리.
			tex = rm->LoadTexture(renderer
				, rawMesh.mtrl.directoryPath, rawMesh.mtrl.texture);
		}
		else
		{
			// 모델파일명에서, 경로정보를 가져온다.
			StrPath dir = modelFileName.GetFilePathExceptFileName();
			tex = rm->LoadTexture2(renderer, dir, rawMesh.mtrl.texture);

			// 텍스쳐 파일을 못찾았다면 media 경로에서 검색한다.
			if (tex && (tex->m_fileName == g_defaultTexture))
			{
				tex = rm->LoadTexture(renderer
					, rawMesh.mtrl.directoryPath, rawMesh.mtrl.texture);
			}
		}

		m_colorMap.push_back(tex);
	}

	if (!rawMesh.mtrl.bumpMap.empty())
		m_normalMap.push_back(
			rm->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.bumpMap));

	if (rawMesh.mtrl.specularMap.empty())
	{
		m_colorMap.push_back(
			rm->LoadTexture2(renderer, "", g_defaultTexture));
	}
	else
	{
		m_specularMap.push_back(
			rm->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.specularMap));
	}

	if (!rawMesh.mtrl.selfIllumMap.empty())
		m_selfIllumMap.push_back(
			rm->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.selfIllumMap));
}


void cMesh::Render( cRenderer &renderer
	, const char *techniqueName
	, cSkeleton *skeleton
	, const XMMATRIX &nodeParentTm // = XMIdentity
	, const XMMATRIX &parentTm //= XMIdentity
)
{
	RET(!IsVisible());
	RET(!m_buffers);

	UpdateConstantBuffer(renderer, techniqueName, skeleton, nodeParentTm, parentTm);
	m_buffers->Render(renderer);
}


void cMesh::RenderInstancing(cRenderer &renderer
	, const char *techniqueName
	, cSkeleton *skeleton
	, const int count
	, const Matrix44 *transforms
	, const XMMATRIX &nodeParentTm //= XMIdentity
	, const XMMATRIX &parentTm //= XMIdentity
)
{
	RET(!IsVisible());
	RET(!m_buffers);

	for (int i = 0; i < count; ++i)
	{
		const XMMATRIX &localTm = m_transform.GetMatrixXM();
		const XMMATRIX transformTm = transforms[i].GetMatrixXM();
		renderer.m_cbInstancing.m_v->worlds[i] 
			= XMMatrixTranspose(localTm * nodeParentTm * transformTm * parentTm);
	}

	UpdateConstantBuffer(renderer, techniqueName, skeleton, XMIdentity, XMIdentity, true);
	m_buffers->RenderInstancing(renderer, count);
}


void cMesh::RenderTessellation(cRenderer &renderer
	, const char *techniqueName
	, const int controlPointCount
	, cSkeleton *skeleton
	, const XMMATRIX &parentTm //= XMIdentity
	, const XMMATRIX &transform //= XMIdentity
)
{
	RET(!IsVisible());
	RET(!m_buffers);

	UpdateConstantBuffer(renderer, techniqueName, skeleton, parentTm, transform, false, true);
	m_buffers->RenderTessellation(renderer, controlPointCount);
}


void cMesh::UpdateConstantBuffer(cRenderer &renderer
	, const char *techniqueName
	, cSkeleton *skeleton
	, const XMMATRIX &nodeParentTm //= XMIdentity
	, const XMMATRIX &parentTm //= XMIdentity
	, const bool isInstancing //= false
	, const bool isTessellation //= false
)
{
	cShader11 *shader = (m_shader)? m_shader : renderer.m_shaderMgr.FindShader(m_buffers->m_vtxType);
	assert(shader);

	if (m_isBeginShader)
	{
		shader->SetTechnique(techniqueName);
		shader->Begin();
		shader->BeginPass(renderer, 0);
	}

	renderer.m_cbClipPlane.Update(renderer, 4);

	const XMMATRIX nodeGlobalTm = m_transform.GetMatrixXM() * nodeParentTm;
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(nodeGlobalTm * parentTm);
	renderer.m_cbPerFrame.Update(renderer);

	renderer.m_cbLight.Update(renderer, 1);
	if (!m_mtrls.empty())
		renderer.m_cbMaterial = m_mtrls[0].GetMaterial();
	renderer.m_cbMaterial.Update(renderer, 2);

	if (isInstancing)
		renderer.m_cbInstancing.Update(renderer, 3);

	if (!m_colorMap.empty())
		if (m_colorMap[0])
			m_colorMap[0]->Bind(renderer, 0);

	if (!m_normalMap.empty())
		if (m_normalMap[0])
			m_normalMap[0]->Bind(renderer, 1);

	if (!m_specularMap.empty())
		if (m_specularMap[0])
			m_specularMap[0]->Bind(renderer, 2);

	if (!m_selfIllumMap.empty())
		if (m_selfIllumMap[0])
			m_selfIllumMap[0]->Bind(renderer, 3);

	// Set Skinning Information
	// 
	for (uint i = 0; i < m_bones.size(); ++i)
	{
		Matrix44 tm = (skeleton) ? (m_bones[i].offsetTm * skeleton->m_tmPose[m_bones[i].id]) : Matrix44::Identity;
		XMMATRIX tfm = XMLoadFloat4x4((XMFLOAT4X4*)&tm) * XMMatrixInverse(NULL, nodeGlobalTm);
		renderer.m_cbSkinning.m_v->mPalette[i] = XMMatrixTranspose(tfm);
	}

	if (!m_bones.empty())
		renderer.m_cbSkinning.Update(renderer, 5);

	if (isTessellation)
		renderer.m_cbTessellation.Update(renderer, 6);
}


void cMesh::CalculateModelVectors(INOUT graphic::sRawMesh2 &mesh)
{
	RET(!mesh.tangent.empty());
	RET(!mesh.binormal.empty());

	const size_t vertexCount = mesh.vertices.size();
	mesh.tangent.resize(vertexCount);
	mesh.binormal.resize(vertexCount);

	//const int faceCount = vertexCount / 3;
	const size_t faceCount = mesh.indices.size() / 3;
	//int index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (size_t i = 0; i<mesh.indices.size(); i+=3)
	{
		// Get the three vertices for this face from the model.
		sVertexNormTex vertex1, vertex2, vertex3;

		const int idx1 = mesh.indices[i];
		const int idx2 = mesh.indices[i+1];
		const int idx3 = mesh.indices[i+2];

		vertex1.p = mesh.vertices[idx1];
		vertex1.n = mesh.normals[idx1];
		vertex1.u = mesh.tex[idx1].x;
		vertex1.v = mesh.tex[idx1].y;

		vertex2.p = mesh.vertices[idx2];
		vertex2.n = mesh.normals[idx2];
		vertex2.u = mesh.tex[idx2].x;
		vertex2.v = mesh.tex[idx2].y;

		vertex3.p = mesh.vertices[idx3];
		vertex3.n = mesh.normals[idx3];
		vertex3.u = mesh.tex[idx3].x;
		vertex3.v = mesh.tex[idx3].y;

		Vector3 tangent, binormal, normal;
		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		normal = tangent.CrossProduct(binormal).Normal();

		// Store the normal, tangent, and binormal for this face back in the model structure.
		mesh.normals[idx3] = normal;
		mesh.tangent[idx3] = tangent;
		mesh.binormal[idx3] = binormal;

		mesh.normals[idx2] = normal;
		mesh.tangent[idx2] = tangent;
		mesh.binormal[idx2] = binormal;

		mesh.normals[idx1] = normal;
		mesh.tangent[idx1] = tangent;
		mesh.binormal[idx1] = binormal;
	}

	return;
}


void cMesh::CalculateTangentBinormal(
	const sVertexNormTex &vertex1
	, const sVertexNormTex &vertex2
	, const sVertexNormTex &vertex3
	, OUT Vector3& tangent
	, OUT Vector3& binormal
)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;

	// Calculate the two vectors for this face.
	vector1[0] = vertex2.p.x - vertex1.p.x;
	vector1[1] = vertex2.p.y - vertex1.p.y;
	vector1[2] = vertex2.p.z - vertex1.p.z;

	vector2[0] = vertex3.p.x - vertex1.p.x;
	vector2[1] = vertex3.p.y - vertex1.p.y;
	vector2[2] = vertex3.p.z - vertex1.p.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.u - vertex1.u;
	tvVector[0] = vertex2.v - vertex1.v;

	tuVector[1] = vertex3.u - vertex1.u;
	tvVector[1] = vertex3.v - vertex1.v;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	tangent.Normalize();
	binormal.Normalize();

	return;
}


void cMesh::Clear()
{
	SAFE_DELETE(m_buffers);

	m_mtrls.clear();
	m_colorMap.clear();
	m_normalMap.clear();
	m_specularMap.clear();
	m_selfIllumMap.clear();
}
