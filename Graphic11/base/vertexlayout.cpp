
#include "stdafx.h"
#include "vertexlayout.h"


using namespace graphic;


cVertexLayout::cVertexLayout()
	: m_vertexLayout(NULL)
	, m_vertexType(0)
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
	m_vertexType = 0;
	for (int i = 0; i < numElements; ++i)
	{
		m_elements.push_back(layout[i]);
		m_elements.back().AlignedByteOffset = size;
		size += BitsPerPixel(layout[i].Format) / 8;

		if (Str32("POSITION") == layout[i].SemanticName)
			m_vertexType |= eVertexType::POSITION;
		else if (Str32("POSITION_RHW") == layout[i].SemanticName)
		{
			m_elements.back().SemanticName = "POSITION"; // recovery, bugfix
			m_vertexType |= eVertexType::POSITION_RHW;
		}
		else if (Str32("NORMAL") == layout[i].SemanticName)
			m_vertexType |= eVertexType::NORMAL;
		else if (Str32("TEXCOORD") == layout[i].SemanticName)
			m_vertexType |= eVertexType::TEXTURE;
		else if (Str32("COLOR") == layout[i].SemanticName)
			m_vertexType |= eVertexType::DIFFUSE;
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
	vector<D3D11_INPUT_ELEMENT_DESC> layout;

	if (!vertices.empty())
	{
		D3D11_INPUT_ELEMENT_DESC element = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout.push_back(element);
	}

	if (!normals.empty())
	{
		D3D11_INPUT_ELEMENT_DESC element = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout.push_back(element);
	}

	if (!tex.empty())
	{
		D3D11_INPUT_ELEMENT_DESC element = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout.push_back(element);
	}

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

	Create(layout);

	//m_elementSize = offset;
}


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
