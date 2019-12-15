
#include "stdafx.h"
#include "temporalbuffer.h"


using namespace graphic;

cTemporalBuffer::cTemporalBuffer()
	: m_buff(NULL)
{

}

cTemporalBuffer::~cTemporalBuffer()
{
	SAFE_RELEASE(m_buff);
}


// Create Temporal Copy Buffer
// readable, writable buffer
// very slow buffer, only use temporal access GPU Buffer
bool cTemporalBuffer::Create(cRenderer &renderer, const int bytesSize, const int stride)
{
	SAFE_RELEASE(m_buff);
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = bytesSize;
	bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	if (FAILED(renderer.GetDevice()->CreateBuffer(&bd, NULL, &m_buff)))
	{
		assert(0);
		return false;
	}

	m_stride = stride;
	m_elementCount = bytesSize / stride;
	return true;
}


bool cTemporalBuffer::Create(cRenderer &renderer, const cVertexBuffer &vtxBuff)
{
	return Create(renderer, vtxBuff.m_sizeOfVertex*vtxBuff.m_vertexCount, vtxBuff.m_sizeOfVertex);
}


bool cTemporalBuffer::Create(cRenderer &renderer, const cIndexBuffer &idxBuff)
{
	const int stride = (idxBuff.m_format == DXGI_FORMAT_R32_UINT) ? 4 : 2;
	return Create(renderer, idxBuff.m_faceCount * 3 * stride, stride);
}


bool cTemporalBuffer::Create(cRenderer &renderer, const cTexture &texture)
{
	const int stride = BitsPerPixel(texture.m_imageInfo.Format) / 8;
	const int bytesSize = texture.m_imageInfo.Width * texture.m_imageInfo.Height * stride;
	return Create(renderer, bytesSize, stride);
}


bool cTemporalBuffer::CopyFrom(cRenderer &renderer, const cVertexBuffer &vtxBuff)
{
	if (m_buff)
	{
		if ((m_elementCount != vtxBuff.m_vertexCount)
			|| (m_stride != vtxBuff.m_sizeOfVertex))
			return false;
	}
	else
	{
		if (!Create(renderer, vtxBuff.m_vertexCount*vtxBuff.m_sizeOfVertex, vtxBuff.m_sizeOfVertex))
			return false;
	}

	renderer.GetDevContext()->CopyResource(m_buff, vtxBuff.m_vtxBuff);
	return true;
}


bool cTemporalBuffer::CopyFrom(cRenderer &renderer, const cIndexBuffer &idxBuff)
{
	const int stride = (idxBuff.m_format == DXGI_FORMAT_R32_UINT) ? 4 : 2;

	if (m_buff)
	{
		if ((m_elementCount != idxBuff.m_faceCount*3)
			|| (m_stride != stride))
			return false;
	}
	else
	{
		if (!Create(renderer, idxBuff.m_faceCount * 3 * stride, stride))
			return false;
	}

	renderer.GetDevContext()->CopyResource(m_buff, idxBuff.m_idxBuff);
	return true;
}


bool cTemporalBuffer::CopyFrom(cRenderer &renderer, const cTexture &texture)
{
	renderer.GetDevContext()->CopyResource(m_buff, texture.m_texture);
	return true;
}


bool cTemporalBuffer::CopyFrom(cRenderer &renderer, ID3D11Texture2D *texture)
{
	renderer.GetDevContext()->CopyResource(m_buff, texture);
	return true;
}


void* cTemporalBuffer::Lock(cRenderer &renderer
	, const D3D11_MAP flag //= D3D11_MAP_READ
)
{
	assert(m_buff);
	D3D11_MAPPED_SUBRESOURCE res;
	ZeroMemory(&res, sizeof(res));
	HRESULT hr = renderer.GetDevContext()->Map(m_buff, 0, flag, 0, &res);
	if (FAILED(hr))
		return NULL;
	return res.pData;
}


void* cTemporalBuffer::Lock(cRenderer &renderer, OUT D3D11_MAPPED_SUBRESOURCE &out)
{
	assert(m_buff);
	ZeroMemory(&out, sizeof(out));
	HRESULT hr = renderer.GetDevContext()->Map(m_buff, 0, D3D11_MAP_READ, 0, &out);
	if (FAILED(hr))
		return NULL;
	return out.pData;
}


void cTemporalBuffer::Unlock(cRenderer &renderer)
{
	assert(m_buff);
	renderer.GetDevContext()->Unmap(m_buff, 0);
}
