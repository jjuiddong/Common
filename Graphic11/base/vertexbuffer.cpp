
#include "stdafx.h"
#include "vertexbuffer.h"

using namespace graphic;


cVertexBuffer::cVertexBuffer() 
	: m_sizeOfVertex(0)
	, m_vertexCount(0)
	, m_vtxBuff(NULL)
{
}

// Copy Constructor
cVertexBuffer::cVertexBuffer(const cVertexBuffer &rhs)
{
	operator = (rhs);
}

cVertexBuffer::~cVertexBuffer()
{
	Clear();
}


bool cVertexBuffer::Create(cRenderer &renderer, const int vertexCount, const int sizeofVertex
	, const D3D11_USAGE usage //= D3D11_USAGE_DEFAULT
)
{
	SAFE_RELEASE(m_vtxBuff);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.ByteWidth = sizeofVertex * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	switch (usage)
	{
	case D3D11_USAGE_DEFAULT: 
		bd.CPUAccessFlags = 0;
		break;
	case D3D11_USAGE_DYNAMIC:
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case D3D11_USAGE_STAGING:
		assert(0); // very slow flag
		break;
	}

	if (FAILED(renderer.GetDevice()->CreateBuffer(&bd, NULL, &m_vtxBuff)))
	{
		assert(0);
		return false;
	}

	m_vertexCount = vertexCount;
	m_sizeOfVertex = sizeofVertex;
	return true;
}


bool cVertexBuffer::Create(cRenderer &renderer, const int vertexCount, const int sizeofVertex
	, const void *vertices
	, const D3D11_USAGE usage //= D3D11_USAGE_DEFAULT
)
{
	SAFE_RELEASE(m_vtxBuff);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.ByteWidth = sizeofVertex * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	switch (usage)
	{
	case D3D11_USAGE_DEFAULT:
		bd.CPUAccessFlags = 0;
		break;
	case D3D11_USAGE_DYNAMIC:
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case D3D11_USAGE_STAGING:
		assert(0); // very slow flag
		break;
	}

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	if (FAILED(renderer.GetDevice()->CreateBuffer(&bd, &InitData, &m_vtxBuff)))
	{
		assert(0);
		return false;
	}

	m_vertexCount = vertexCount;
	m_sizeOfVertex = sizeofVertex;
	return true;
}


void* cVertexBuffer::Lock(cRenderer &renderer
	, const D3D11_MAP flag //= D3D11_MAP_WRITE_DISCARD
)
{
	RETV(!m_vtxBuff, NULL);

	D3D11_MAPPED_SUBRESOURCE res;
	ZeroMemory(&res, sizeof(res));
	HRESULT hr = renderer.GetDevContext()->Map(m_vtxBuff, 0, flag, 0, &res);
	if (FAILED(hr))
		return NULL;
	return res.pData;
}


void cVertexBuffer::Unlock(cRenderer &renderer)
{
	RET(!m_vtxBuff);
	renderer.GetDevContext()->Unmap(m_vtxBuff, 0);
}


void cVertexBuffer::Bind(cRenderer &renderer) const
{
	const UINT offset = 0;
	const UINT stride = (UINT)m_sizeOfVertex;
	renderer.GetDevContext()->IASetVertexBuffers(0, 1, &m_vtxBuff, &stride, &offset);
}


inline DWORD FtoDW(FLOAT f) { return *((DWORD*)&f); }
void cVertexBuffer::RenderLineStrip(cRenderer &renderer)
{
}


void cVertexBuffer::RenderLineList(cRenderer &renderer)
{
}

// ZBuffer Enable
void cVertexBuffer::RenderLineList2(cRenderer &renderer)
{
}


void cVertexBuffer::RenderPointList2(cRenderer &renderer, const int count) // count=0
{
}


void cVertexBuffer::RenderPointList(cRenderer &renderer, const int count) // count=0
{
}


void cVertexBuffer::RenderTriangleStrip(cRenderer &renderer)
{
}


void cVertexBuffer::RenderTriangleList(cRenderer &renderer)
{
}


void cVertexBuffer::Clear()
{
	m_vertexCount = 0;
	m_sizeOfVertex = 0;
	SAFE_RELEASE(m_vtxBuff);
}


void cVertexBuffer::Set(cRenderer &renderer, const cVertexBuffer &rhs)
{
	if (this != &rhs)
	{
		Clear();

		m_sizeOfVertex = rhs.m_sizeOfVertex;
		m_vertexCount = rhs.m_vertexCount;
		Create(renderer, rhs.m_vertexCount, rhs.m_sizeOfVertex);		
		renderer.GetDevContext()->CopyResource(m_vtxBuff, rhs.m_vtxBuff);
	}
}


cVertexBuffer& cVertexBuffer::operator=(const cVertexBuffer &rhs)
{
	if (this != &rhs)
	{
		Clear();

		m_sizeOfVertex = rhs.m_sizeOfVertex;
		m_vertexCount = rhs.m_vertexCount;
		m_vtxBuff = NULL; // No Copy
	}
	return *this;
}
