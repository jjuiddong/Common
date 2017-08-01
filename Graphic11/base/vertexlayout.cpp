
#include "stdafx.h"
#include "vertexlayout.h"


using namespace graphic;


cVertexLayout::cVertexLayout()
	: m_vertexLayout(NULL)
{
}

cVertexLayout::~cVertexLayout()
{
	SAFE_RELEASE(m_vertexLayout);
}


bool cVertexLayout::Create(cRenderer &renderer, ID3DBlob *vsBlob 
	, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements)
{
	Create(layout, numElements);

	if (FAILED(renderer.GetDevice()->CreateInputLayout((D3D11_INPUT_ELEMENT_DESC*)&m_elements[0], m_elements.size()
		, vsBlob->GetBufferPointer()
		, vsBlob->GetBufferSize()
		, &m_vertexLayout)))
		return false;

	return true;
}


bool cVertexLayout::Create(cRenderer &renderer, const BYTE *pIAInputSignature, const SIZE_T IAInputSignatureSize
	, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements)
{
	Create(layout, numElements);

	if (FAILED(renderer.GetDevice()->CreateInputLayout((D3D11_INPUT_ELEMENT_DESC*)&m_elements[0], m_elements.size()
		, pIAInputSignature
		, IAInputSignatureSize
		, &m_vertexLayout)))
		return false;

	return true;
}


bool cVertexLayout::Create(const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements)
{
	int size = 0;
	m_elements.clear();
	for (int i = 0; i < numElements; ++i)
	{
		m_elements.push_back(layout[i]);
		m_elements.back().AlignedByteOffset = size;
		size += BitsPerPixel(layout[i].Format) / 8;
	}

	m_elementSize = size;
	return true;
}


bool cVertexLayout::Create(const vector<D3D11_INPUT_ELEMENT_DESC> &layout)
{
	int size = 0;
	m_elements.clear();
	for (u_int i = 0; i < layout.size(); ++i)
	{
		m_elements.push_back(layout[i]);
		m_elements.back().AlignedByteOffset = size;
		size += BitsPerPixel(layout[i].Format) / 8;
	}

	m_elementSize = size;
	return true;
}


bool cVertexLayout::Create(const sRawMesh &rawMesh )
{
	CreateDecl(rawMesh.vertices,
		rawMesh.normals,
		rawMesh.tex,
		rawMesh.tangent,
		rawMesh.binormal,
		rawMesh.weights);

	return true;
}


bool cVertexLayout::Create(const sRawMesh2 &rawMesh)
{
	CreateDecl(rawMesh.vertices,
		rawMesh.normals,
		rawMesh.tex,
		rawMesh.tangent,
		rawMesh.binormal,
		rawMesh.weights);

	return true;
}


void cVertexLayout::CreateDecl(
	const vector<Vector3> &vertices,
	const vector<Vector3> &normals,
	const vector<Vector3> &tex,
	const vector<Vector3> &tangent,
	const vector<Vector3> &binormal,
	const vector<sVertexWeight> &weights
	)
{
	int offset = 0;
	m_elements.clear();

	//if (!vertices.empty())
	//{
	//	D3DVERTEXELEMENT9 element = { 0,  (WORD)offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 };
	//	m_decl.push_back(element);
	//	offset += 12;
	//}

	//if (!normals.empty())
	//{
	//	D3DVERTEXELEMENT9 element = { 0, (WORD)offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 };
	//	m_decl.push_back(element);
	//	offset += 12;
	//}

	//if (!tex.empty())
	//{
	//	D3DVERTEXELEMENT9 element = { 0, (WORD)offset, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 };
	//	m_decl.push_back(element);
	//	offset += 8;
	//}

	//if (!tangent.empty())
	//{
	//	D3DVERTEXELEMENT9 element = { 0, (WORD)offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 };
	//	m_decl.push_back(element);
	//	offset += 12;
	//}

	//if (!binormal.empty())
	//{
	//	D3DVERTEXELEMENT9 element = { 0, (WORD)offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL,  0 };
	//	m_decl.push_back(element);
	//	offset += 12;
	//}

	//if (!weights.empty())
	//{
	//	// 텍스쳐 좌표에 블랜딩 정보를 저장한다.
	//	// blend Weight 4
	//	D3DVERTEXELEMENT9 element1 = { 0, (WORD)offset, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1 };
	//	m_decl.push_back(element1);
	//	offset += 16;

	//	// blend Indices 4
	//	D3DVERTEXELEMENT9 element2 = { 0, (WORD)offset, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  2 };
	//	m_decl.push_back(element2);
	//	offset += 16;
	//}

	//D3DVERTEXELEMENT9 endElement = D3DDECL_END();
	//m_decl.push_back(endElement);

	//m_elementSize = offset;
}


//int cVertexLayout::GetOffset( const BYTE usage, const BYTE usageIndex ) const //usageIndex=0
//{
//	//for (u_int i=0; i < m_decl.size(); ++i)
//	//{
//	//	if ((usage == m_decl[ i].Usage) && (usageIndex == m_decl[ i].UsageIndex))
//	//		return m_decl[ i].Offset;
//	//}
//	return -1;
//}


void cVertexLayout::Bind(cRenderer &renderer)
{
	assert(m_vertexLayout);
	renderer.GetDevContext()->IASetInputLayout(m_vertexLayout);
}


int cVertexLayout::GetOffset(const char *semanticName) const
{
	int offset = 0;
	for (auto &elem : m_elements)
	{
		if (!strcmp(elem.SemanticName, semanticName))
			break;
		offset += BitsPerPixel(elem.Format) / 8;
	}

	return offset;
}
