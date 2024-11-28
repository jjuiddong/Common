
#include "stdafx.h"
#include "indexbuffer.h"

using namespace graphic;


cIndexBuffer::cIndexBuffer() :
	m_idxBuff(NULL)
	, m_faceCount(0)
	, m_primitiveSize(3)
	, m_format(DXGI_FORMAT_R16_UINT)
{
}

// Copy Constructor
cIndexBuffer::cIndexBuffer(const cIndexBuffer &rhs)
{
	operator = (rhs);
}


cIndexBuffer::~cIndexBuffer()
{
	Clear();
}


bool cIndexBuffer::Create(cRenderer &renderer, const int faceCount
	, const D3D11_USAGE usage //= D3D11_USAGE_DEFAULT
	, const DXGI_FORMAT fmt //= DXGI_FORMAT_R16_UINT
	, const int primitiveSize //= 3
)
{
	SAFE_RELEASE(m_idxBuff);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.ByteWidth = sizeof(WORD) * faceCount * primitiveSize;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
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

	if (FAILED(renderer.GetDevice()->CreateBuffer(&bd, NULL, &m_idxBuff)))
		return false;

	m_faceCount = faceCount;
	m_primitiveSize = primitiveSize;
	m_format = fmt;
	return true;
}


bool cIndexBuffer::Create(cRenderer &renderer, const int faceCount, BYTE *indices
	, const D3D11_USAGE usage //= D3D11_USAGE_DEFAULT
	, const DXGI_FORMAT fmt //= DXGI_FORMAT_R16_UINT
	, const int primitiveSize //= 3
)
{
	SAFE_RELEASE(m_idxBuff);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.ByteWidth = (uint)BitsPerPixel(fmt)/8 * faceCount * primitiveSize;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
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
	InitData.pSysMem = indices;

	if (FAILED(renderer.GetDevice()->CreateBuffer(&bd, &InitData, &m_idxBuff)))
		return false;

	m_faceCount = faceCount;
	m_primitiveSize = primitiveSize;
	m_format = fmt;
	return true;
}


bool cIndexBuffer::Create2(cRenderer &renderer
	, const int primitiveCount, const int primitiveSize)
{
	SAFE_RELEASE(m_idxBuff);

	//if (FAILED(renderer.GetDevice()->CreateIndexBuffer(primitiveCount * primitiveSize * sizeof(WORD),
	//	D3DUSAGE_WRITEONLY,
	//	D3DFMT_INDEX16,
	//	D3DPOOL_MANAGED,
	//	&m_idxBuff, NULL)))
	//{
	//	return false;
	//}

	m_faceCount = primitiveCount;
	return true;
}


void* cIndexBuffer::Lock(cRenderer &renderer
	, const D3D11_MAP flag //= D3D11_MAP_WRITE_DISCARD
)
{
	assert(0); // 쓰기 모드로만 되어있기 때문에, Lock을 걸면 정보가 사라진다. 일단 쓰지말것

	RETV(!m_idxBuff, NULL);

	D3D11_MAPPED_SUBRESOURCE res;
	ZeroMemory(&res, sizeof(res));
	HRESULT hr = renderer.GetDevContext()->Map(m_idxBuff, 0, flag, 0, &res);
	if (FAILED(hr))
		return NULL;
	return res.pData;
}


void cIndexBuffer::Unlock(cRenderer &renderer)
{
	RET(!m_idxBuff);
	renderer.GetDevContext()->Unmap(m_idxBuff, 0);
}


void cIndexBuffer::Bind(cRenderer &renderer) const
{
	renderer.GetDevContext()->IASetIndexBuffer(m_idxBuff, m_format, 0);
}


void cIndexBuffer::Clear()
{
	m_faceCount = 0;
	SAFE_RELEASE(m_idxBuff);
}


void cIndexBuffer::Set(cRenderer &renderer, const cIndexBuffer &rhs)
{
	if (this != &rhs)
	{
		Clear();

		m_format = rhs.m_format;
		m_faceCount = rhs.m_faceCount;
		Create(renderer, rhs.m_faceCount, D3D11_USAGE_DEFAULT, rhs.m_format);
		renderer.GetDevContext()->CopyResource(m_idxBuff, rhs.m_idxBuff);
	}
}


cIndexBuffer& cIndexBuffer::operator=(const cIndexBuffer &rhs)
{
	if (this != &rhs)
	{
		Clear();

		m_format = rhs.m_format;
		m_faceCount = rhs.m_faceCount;
		m_idxBuff = NULL; // No Copy
	}
	return *this;
}
