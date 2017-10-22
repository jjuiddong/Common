
#include "stdafx.h"
#include "mesh.h"
#include "skeleton.h"

using namespace graphic;


cMesh::cMesh()
	: m_buffers(NULL)
	, m_skeleton(NULL)
	, m_isShow(true)
	, m_isHilight(false)
{
}

cMesh::~cMesh()
{
	Clear();
}


// Create Mesh
bool cMesh::Create(cRenderer &renderer, INOUT sRawMesh2 &mesh, cSkeleton *skeleton
	, const bool calculateTangentBinormal //= false
)
{
	Clear();

	m_name = mesh.name;
	m_skeleton = skeleton;
	m_bones = mesh.bones;

	CreateMaterials(renderer, mesh);
	
	m_buffers = new cMeshBuffer(renderer, mesh);

	return true;
}


void cMesh::CreateMaterials(cRenderer &renderer, const sRawMesh2 &rawMesh)
{
	m_mtrls.push_back(cMaterial());
	m_mtrls.back().Init(rawMesh.mtrl);

	if (rawMesh.mtrl.texture.empty())
	{
		m_colorMap.push_back(
			cResourceManager::Get()->LoadTexture2(renderer, "", g_defaultTexture));
	}
	else
	{
		m_colorMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.texture));
	}

	if (!rawMesh.mtrl.bumpMap.empty())
		m_normalMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.bumpMap));

	if (rawMesh.mtrl.specularMap.empty())
	{
		m_colorMap.push_back(
			cResourceManager::Get()->LoadTexture2(renderer, "", g_defaultTexture));
	}
	else
	{
		m_specularMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.specularMap));
	}

	if (!rawMesh.mtrl.selfIllumMap.empty())
		m_selfIllumMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.selfIllumMap));
}


void cMesh::Render( cRenderer &renderer
	, const char *techniqueName
	, const XMMATRIX &parentTm // = XMIdentity
	, const XMMATRIX &transform //= XMIdentity
)
{
	RET(!m_isShow);
	RET(!m_buffers);

	UpdateConstantBuffer(renderer, techniqueName, parentTm, transform);
	m_buffers->Render(renderer);
}


void cMesh::RenderInstancing(cRenderer &renderer
	, const char *techniqueName
	, const int count
	, const XMMATRIX &parentTm //= XMIdentity
)
{
	RET(!m_isShow);
	RET(!m_buffers);

	UpdateConstantBuffer(renderer, techniqueName, parentTm, XMIdentity);
	m_buffers->RenderInstancing(renderer, count);
}


void cMesh::UpdateConstantBuffer(cRenderer &renderer
	, const char *techniqueName
	, const XMMATRIX &parentTm //= XMIdentity
	, const XMMATRIX &transform //= XMIdentity
)
{
	cShader11 *shader = renderer.m_shaderMgr.FindShader(m_buffers->m_vtxType);
	assert(shader);
	shader->SetTechnique(techniqueName);
	shader->Begin();
	shader->BeginPass(renderer, 0);
	renderer.m_cbClipPlane.Update(renderer, 4);

	Matrix44 worldM;
	worldM.SetPosition(Vector3(40, 0, 40));

	const XMMATRIX nodeGlobalTm = m_transform.GetMatrixXM() * parentTm;
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(nodeGlobalTm * transform);
	renderer.m_cbPerFrame.Update(renderer);

	renderer.m_cbLight.Update(renderer, 1);
	if (!m_mtrls.empty())
	{
		renderer.m_cbMaterial = m_mtrls[0].GetMaterial();
		if (m_isHilight)
		{
			renderer.m_cbMaterial.m_v->ambient = XMVectorSet(0.3f, 0.3f, 1.f, 1);
			renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(0.3f, 0.3f, 1.f, 1);
		}
	}
	renderer.m_cbMaterial.Update(renderer, 2);

	if (!m_colorMap.empty())
		m_colorMap[0]->Bind(renderer, 0);

	if (!m_normalMap.empty())
		m_normalMap[0]->Bind(renderer, 1);

	if (!m_specularMap.empty())
		m_specularMap[0]->Bind(renderer, 2);

	if (!m_selfIllumMap.empty())
		m_selfIllumMap[0]->Bind(renderer, 3);

	// Set Skinning Information
	// 
	for (u_int i = 0; i < m_bones.size(); ++i)
	{
		Matrix44 tm = (m_skeleton) ? (m_bones[i].offsetTm * m_skeleton->m_tmPose[m_bones[i].id]) : Matrix44::Identity;
		XMMATRIX tfm = XMLoadFloat4x4((XMFLOAT4X4*)&tm) * XMMatrixInverse(NULL, nodeGlobalTm);
		renderer.m_cbSkinning.m_v->mPalette[i] = XMMatrixTranspose(tfm);
	}

	if (!m_bones.empty())
		renderer.m_cbSkinning.Update(renderer, 5);
}


void cMesh::CalculateModelVectors(INOUT graphic::sRawMesh2 &mesh)
{
	RET(!mesh.tangent.empty());
	RET(!mesh.binormal.empty());

	const int vertexCount = mesh.vertices.size();
	mesh.tangent.resize(vertexCount);
	mesh.binormal.resize(vertexCount);

	//const int faceCount = vertexCount / 3;
	const int faceCount = mesh.indices.size() / 3;
	//int index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (u_int i = 0; i<mesh.indices.size(); i+=3)
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
