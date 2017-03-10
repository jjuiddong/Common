
#include "stdafx.h"
#include "vertexdeclaration.h"


using namespace graphic;


cVertexDeclaration::cVertexDeclaration()
{
}

cVertexDeclaration::~cVertexDeclaration()
{
}


bool cVertexDeclaration::Create(const D3DVERTEXELEMENT9 decl[])
{
	m_decl.clear();

	int size = 0;
	for (int i = 0; i < 128; ++i)
	{
		if (D3DDECLTYPE_UNUSED == decl[i].Type)
			break;

		m_decl.push_back(decl[i]);

		switch (decl[i].Type)
		{
		case D3DDECLTYPE_FLOAT1: size = decl[i].Offset + sizeof(float) * 1; break;
		case D3DDECLTYPE_FLOAT2: size = decl[i].Offset + sizeof(float) * 2; break;
		case D3DDECLTYPE_FLOAT3: size = decl[i].Offset + sizeof(float)*3; break;
		case D3DDECLTYPE_FLOAT4: size = decl[i].Offset + sizeof(float) * 4; break;
		case D3DDECLTYPE_D3DCOLOR: size = decl[i].Offset + sizeof(DWORD); break;
		}
	}

	m_elementSize = size;
	return true;
}


bool cVertexDeclaration::Create(const sRawMesh &rawMesh )
{
	CreateDecl(rawMesh.vertices,
		rawMesh.normals,
		rawMesh.tex,
		rawMesh.tangent,
		rawMesh.binormal,
		rawMesh.weights);

	return true;
}


bool cVertexDeclaration::Create(const sRawMesh2 &rawMesh)
{
	CreateDecl(rawMesh.vertices,
		rawMesh.normals,
		rawMesh.tex,
		rawMesh.tangent,
		rawMesh.binormal,
		rawMesh.weights);

	return true;
}


void cVertexDeclaration::CreateDecl(
	const vector<Vector3> &vertices,
	const vector<Vector3> &normals,
	const vector<Vector3> &tex,
	const vector<Vector3> &tangent,
	const vector<Vector3> &binormal,
	const vector<sVertexWeight> &weights
	)
{
	int offset = 0;
	m_decl.clear();

	if (!vertices.empty())
	{
		D3DVERTEXELEMENT9 element = { 0,  (WORD)offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 };
		m_decl.push_back(element);
		offset += 12;
	}

	if (!normals.empty())
	{
		D3DVERTEXELEMENT9 element = { 0, (WORD)offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 };
		m_decl.push_back(element);
		offset += 12;
	}

	if (!tex.empty())
	{
		D3DVERTEXELEMENT9 element = { 0, (WORD)offset, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 };
		m_decl.push_back(element);
		offset += 8;
	}

	if (!tangent.empty())
	{
		D3DVERTEXELEMENT9 element = { 0, (WORD)offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 };
		m_decl.push_back(element);
		offset += 12;
	}

	if (!binormal.empty())
	{
		D3DVERTEXELEMENT9 element = { 0, (WORD)offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL,  0 };
		m_decl.push_back(element);
		offset += 12;
	}

	if (!weights.empty())
	{
		// 텍스쳐 좌표에 블랜딩 정보를 저장한다.
		// blend Weight 4
		D3DVERTEXELEMENT9 element1 = { 0, (WORD)offset, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1 };
		m_decl.push_back(element1);
		offset += 16;

		// blend Indices 4
		D3DVERTEXELEMENT9 element2 = { 0, (WORD)offset, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  2 };
		m_decl.push_back(element2);
		offset += 16;
	}

	D3DVERTEXELEMENT9 endElement = D3DDECL_END();
	m_decl.push_back(endElement);

	m_elementSize = offset;
}


int cVertexDeclaration::GetOffset( const BYTE usage, const BYTE usageIndex ) const //usageIndex=0
{
	for (u_int i=0; i < m_decl.size(); ++i)
	{
		if ((usage == m_decl[ i].Usage) && (usageIndex == m_decl[ i].UsageIndex))
			return m_decl[ i].Offset;
	}
	return -1;
}
