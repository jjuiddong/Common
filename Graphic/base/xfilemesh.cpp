
#include "stdafx.h"
#include "xfilemesh.h"


using namespace graphic;


cXFileMesh::cXFileMesh()
	: m_mesh(NULL)
	, m_shader(NULL)
	, m_materials(NULL)
	, m_textures(NULL)
{
}

cXFileMesh::~cXFileMesh()
{
	Clear();
}


bool cXFileMesh::Create(cRenderer &renderer, const string &fileName, const bool isShadow)
{
	m_fileName = fileName;
	LPD3DXBUFFER pAdjacencyBuffer = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	HRESULT hr;

	Clear();

	// Load the mesh
	if (FAILED(hr = D3DXLoadMeshFromXA(fileName.c_str(), D3DXMESH_32BIT | D3DXMESH_MANAGED, renderer.GetDevice(),
		&pAdjacencyBuffer, &pMtrlBuffer, NULL,
		(DWORD*)&m_materialsCount, &m_mesh)))
	{
		return false;
	}

	// Optimize the mesh for performance
	if (FAILED(hr = m_mesh->OptimizeInplace(
		D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL)))
	{
		SAFE_RELEASE(pAdjacencyBuffer);
		SAFE_RELEASE(pMtrlBuffer);
		return false;
	}

	D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
	hr = CreateMaterials(fileName, renderer, d3dxMtrls, m_materialsCount);

	SAFE_RELEASE(pAdjacencyBuffer);
	SAFE_RELEASE(pMtrlBuffer);

	// Extract data from m_pMesh for easy access
	//D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
	m_verticesCount = m_mesh->GetNumVertices();
	m_facesCount = m_mesh->GetNumFaces();
	m_stride = m_mesh->GetNumBytesPerVertex();
	//m_mesh->GetIndexBuffer(&m_pIB);
	//m_mesh->GetVertexBuffer(&m_pVB);
	//m_mesh->GetDeclaration(decl);
	//m_renderer.GetDevice()->CreateVertexDeclaration(decl, &m_pDecl);

	InitBoundingBox();

	if (isShadow)
		m_shadow.Create(renderer, m_mesh);

	return true;
}


HRESULT cXFileMesh::CreateMaterials(const string &filePath, cRenderer &renderer, D3DXMATERIAL* d3dxMtrls, const DWORD dwNumMaterials)
{
	//common::dbg::Print("CreateMaterials %d --------- \n", dwNumMaterials);

	m_materialsCount = dwNumMaterials;
	if (d3dxMtrls && m_materialsCount > 0)
	{
		m_materials = new cMaterial[dwNumMaterials];
		if (m_materials == NULL)
			return E_OUTOFMEMORY;
		m_textures = new cTexture*[dwNumMaterials];
		if (m_textures == NULL)
			return E_OUTOFMEMORY;
		ZeroMemory(m_textures, sizeof(cTexture*)*dwNumMaterials);

		for (DWORD i = 0; i < dwNumMaterials; i++)
		{
			m_materials[i].m_mtrl = d3dxMtrls[i].MatD3D;
			if (d3dxMtrls[i].pTextureFilename)
			{
				string textureFileName = common::GetFilePathExceptFileName(filePath) + "/" + d3dxMtrls[i].pTextureFilename;
				//common::dbg::Print("texture file name = %s\n", textureFileName.c_str());
				m_textures[i] = cResourceManager::Get()->LoadTexture(renderer, textureFileName);
				if (m_textures[i])
				{
					//common::dbg::Print("\t- Loading Error!! texture file name = %s\n", textureFileName.c_str());
				}
			}
			else
			{
				m_textures[i] = cResourceManager::Get()->LoadTexture(renderer, "whitetex.dds");
			}
		}
	}
	return S_OK;
}


void cXFileMesh::InitBoundingBox()
{
	if (!m_mesh)
		return;

	Vector3 minP = Vector3::Max;
	Vector3 maxP = Vector3::Min;
	const int stride = m_mesh->GetNumBytesPerVertex();
	const int vertexCount = m_mesh->GetNumVertices();

	BYTE *pvtx = NULL;
	m_mesh->LockVertexBuffer(0, (void**)&pvtx);
	for (int i = 0; i < vertexCount; ++i)
	{
		Vector3 p = *(Vector3*)pvtx;
		minP = minP.Minimum(p);
		maxP = maxP.Maximum(p);
		pvtx += stride;
	}
	m_mesh->UnlockVertexBuffer();

	m_boundingBox.SetBoundingBox(minP, maxP);
}


bool cXFileMesh::Update(const float deltaSeconds)
{
	return true;
}


void cXFileMesh::Render(cRenderer &renderer)
{
	RET(!m_mesh);

	if (m_shader)
	{
		const int passCount = m_shader->Begin();
		for (int i = 0; i < passCount; ++i)
		{
			m_shader->BeginPass(i);
			for (int i = 0; i < m_materialsCount; i++)
			{
				//const cLight &mainLight = cLightManager::Get()->GetMainLight();
				//mainLight.Bind(*m_shader);
				//m_shader->SetVector("g_vEyePos", cMainCamera::Get()->GetEyePos());

				m_materials[i].Bind(*m_shader);

				if (m_textures[i])
					m_textures[i]->Bind(*m_shader, "g_colorMapTexture");

				m_shader->SetMatrix("g_mWorld", m_tm);
				m_shader->CommitChanges();
				m_mesh->DrawSubset(i);
			}
			m_shader->EndPass();
		}
		m_shader->End();
	}
	else
	{
		renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&m_tm);

		for (int i = 0; i < m_materialsCount; i++)
		{
			m_materials[i].Bind(renderer);
			if (m_textures[i])
				m_textures[i]->Bind(renderer, 0);
			m_mesh->DrawSubset(i);
		}
	}
}


void cXFileMesh::RenderShadow(cRenderer &renderer)
{
	RET(!m_shadow.m_mesh);

	m_shader->SetMatrix("g_mWorld", Matrix44::Identity);

	const int passCnt = m_shader->Begin();
	for (int i = 0; i < passCnt; ++i)
	{
		m_shader->BeginPass(i);
		m_shader->CommitChanges();
		m_shadow.Render(renderer);
		m_shader->EndPass();
	}
	m_shader->End();
}


void cXFileMesh::Clear()
{
	SAFE_RELEASE(m_mesh);
	SAFE_DELETEA(m_materials);
	SAFE_DELETEA(m_textures);
}


bool cXFileMesh::IsLoad()
{
	return m_mesh ? true : false;
}
