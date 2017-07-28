
#include "stdafx.h"
#include "indexbuffer.h"

using namespace graphic;


cIndexBuffer::cIndexBuffer() :
	m_idxBuff(NULL)
,	m_faceCount(0)
{
}

cIndexBuffer::~cIndexBuffer()
{
	Clear();
}


bool cIndexBuffer::Create(cRenderer &renderer, int faceCount)
{
	SAFE_RELEASE(m_idxBuff);

	//if (FAILED(renderer.GetDevice()->CreateIndexBuffer(faceCount*3*sizeof(WORD), 
	//	D3DUSAGE_WRITEONLY,
	//	D3DFMT_INDEX16,
	//	D3DPOOL_MANAGED,
	//	&m_idxBuff, NULL)))
	//{
	//	return false;
	//}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * faceCount * 3;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	if (FAILED(renderer.GetDevice()->CreateBuffer(&bd, NULL, &m_idxBuff)))
		return false;

	m_faceCount = faceCount;
	return true;
}


bool cIndexBuffer::Create(cRenderer &renderer, int faceCount, WORD *indices)
{
	SAFE_RELEASE(m_idxBuff);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * faceCount * 3;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;

	if (FAILED(renderer.GetDevice()->CreateBuffer(&bd, &InitData, &m_idxBuff)))
		return false;

	m_faceCount = faceCount;
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


void* cIndexBuffer::Lock()
{
	RETV(!m_idxBuff, NULL);

	WORD *indices = NULL;
	//m_idxBuff->Lock(0, 0, (void**)&indices, 0);
	return indices;
}


void cIndexBuffer::Unlock()
{
	//m_idxBuff->Unlock();
}


void cIndexBuffer::Bind(cRenderer &renderer) const
{
	renderer.GetDevContext()->IASetIndexBuffer(m_idxBuff, DXGI_FORMAT_R16_UINT, 0);
}


void cIndexBuffer::Clear()
{
	m_faceCount = 0;
	SAFE_RELEASE(m_idxBuff);
}


void cIndexBuffer::Set(cRenderer &renderer, cIndexBuffer &rhs)
{
	if (this != &rhs)
	{
		m_faceCount = rhs.m_faceCount;

		if (Create(renderer, rhs.m_faceCount))
		{
			if (BYTE* dest = (BYTE*)Lock())
			{
				if (BYTE *src = (BYTE*)rhs.Lock())
				{
					memcpy(dest, src, rhs.m_faceCount*3*sizeof(WORD));
					rhs.Unlock();
				}
				Unlock();
			}
		}
	}
}
