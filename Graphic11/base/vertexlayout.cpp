
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
	Clear();
}


bool cVertexLayout::Create(cRenderer &renderer, ID3DBlob *vsBlob 
	, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements)
{
	Clear();

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
	Clear();

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
		else if (Str32("TANGENT") == layout[i].SemanticName)
			m_vertexType |= eVertexType::TANGENT;
		else if (Str32("BINORMAL") == layout[i].SemanticName)
			m_vertexType |= eVertexType::BINORMAL;
		else if (Str32("BLENDINDICES") == layout[i].SemanticName)
			m_vertexType |= eVertexType::BLENDINDICES;
		else if (Str32("BLENDWEIGHT") == layout[i].SemanticName)
			m_vertexType |= eVertexType::BLENDWEIGHT;
	}

	m_elementSize = size;
	return true;
}


bool cVertexLayout::Create(const vector<D3D11_INPUT_ELEMENT_DESC> &layout)
{
	RETV(layout.empty(), false);
	return Create(&layout[0], layout.size());
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

	if (!tangent.empty())
	{
		D3D11_INPUT_ELEMENT_DESC element = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout.push_back(element);
	}

	if (!binormal.empty())
	{
		D3D11_INPUT_ELEMENT_DESC element = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout.push_back(element);
	}

	if (!weights.empty())
	{
		// 텍스쳐 좌표에 블랜딩 정보를 저장한다.
		// blend Weight 4
		D3D11_INPUT_ELEMENT_DESC element1 = { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout.push_back(element1);

		D3D11_INPUT_ELEMENT_DESC element2 = { "BLENDWEIGHT", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout.push_back(element2);
	}

	Create(layout);
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


void cVertexLayout::Clear()
{
	SAFE_RELEASE(m_vertexLayout);
}
