
#include "stdafx.h"
#include "vertexbuffer.h"

using namespace graphic;


cVertexBuffer::cVertexBuffer() 
	: m_sizeOfVertex(0)
	, m_vertexCount(0)
	, m_vtxBuff(NULL)
{

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


bool cVertexBuffer::Create(cRenderer &renderer, const int vertexCount, const int sizeofVertex, void *vertices
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
	assert(m_vtxBuff);

	D3D11_MAPPED_SUBRESOURCE res;
	ZeroMemory(&res, sizeof(res));
	HRESULT hr = renderer.GetDevContext()->Map(m_vtxBuff, 0, flag, 0, &res);
	if (FAILED(hr))
		return NULL;
	return res.pData;
}


void cVertexBuffer::Unlock(cRenderer &renderer)
{
	assert(m_vtxBuff);
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
	//RET(!m_vtxBuff);

	//renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&Matrix44::Identity);

	//DWORD lighting;
	//renderer.GetDevice()->GetRenderState(D3DRS_LIGHTING, &lighting);
	//renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	//renderer.GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);

	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(10));
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(1.0f));
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.0f));
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f));
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.0f));

	//Bind(renderer);
	//renderer.GetDevice()->DrawPrimitive(D3DPT_LINESTRIP, 0, m_vertexCount - 1);
	//renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lighting);
	//renderer.GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
}


void cVertexBuffer::RenderLineList(cRenderer &renderer)
{
	//RET(!m_vtxBuff);

	//renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&Matrix44::Identity);

	//DWORD lighting;
	//renderer.GetDevice()->GetRenderState(D3DRS_LIGHTING, &lighting);
	//renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	//renderer.GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	//Bind(renderer);
	//renderer.GetDevice()->DrawPrimitive(D3DPT_LINELIST, 0, m_vertexCount /2);
	//renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lighting);
	//renderer.GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
}

// ZBuffer Enable
void cVertexBuffer::RenderLineList2(cRenderer &renderer)
{
	//RET(!m_vtxBuff);

	//DWORD lighting;
	//renderer.GetDevice()->GetRenderState(D3DRS_LIGHTING, &lighting);
	//renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	//Bind(renderer);
	//renderer.GetDevice()->DrawPrimitive(D3DPT_LINELIST, 0, m_vertexCount / 2);
	//renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lighting);
}


void cVertexBuffer::RenderPointList2(cRenderer &renderer, const int count) // count=0
{
	//RET(!m_vtxBuff);

	//DWORD lighting;
	//renderer.GetDevice()->GetRenderState(D3DRS_LIGHTING, &lighting);
	//renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	//renderer.GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);

	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(10));
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(1.0f));
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.0f));
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f));
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.0f));

	////renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&Matrix44::Identity);

	//Bind(renderer);
	//renderer.GetDevice()->DrawPrimitive(D3DPT_POINTLIST, 0, (count == 0) ? m_vertexCount : count);

	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	//renderer.GetDevice()->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	//renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lighting);
	//renderer.GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
}


void cVertexBuffer::RenderPointList(cRenderer &renderer, const int count) // count=0
{
	//RET(!m_vtxBuff);

	//renderer.GetDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&Matrix44::Identity );

	//Bind(renderer);
	//renderer.GetDevice()->DrawPrimitive(D3DPT_POINTLIST, 0, (count == 0) ? m_vertexCount : count);
}


void cVertexBuffer::RenderTriangleStrip(cRenderer &renderer)
{
	//RET(!m_vtxBuff);
	Bind(renderer);
	//renderer.GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_vertexCount - 2);
}


void cVertexBuffer::RenderTriangleList(cRenderer &renderer)
{
	//RET(!m_vtxBuff);
	Bind(renderer);
	//renderer.GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_vertexCount/3);
}


void cVertexBuffer::Clear()
{
	m_vertexCount = 0;
	m_sizeOfVertex = 0;
	SAFE_RELEASE(m_vtxBuff);
}


//cVertexBuffer& cVertexBuffer::operator=(cVertexBuffer &rhs)
void cVertexBuffer::Set(cRenderer &renderer, cVertexBuffer &rhs)
{
	if (this != &rhs)
	{
		m_sizeOfVertex = rhs.m_sizeOfVertex;
		m_vertexCount = rhs.m_vertexCount;
		
		//if (Create(renderer, rhs.m_vertexCount, rhs.m_sizeOfVertex, rhs.m_fvf))
		if (Create(renderer, rhs.m_vertexCount, rhs.m_sizeOfVertex))
		{
			//if (BYTE* dest = (BYTE*)Lock())
			//{
			//	if (BYTE *src = (BYTE*)rhs.Lock())
			//	{
			//		memcpy(dest, src, rhs.m_vertexCount*m_sizeOfVertex);
			//		rhs.Unlock();
			//	}
			//	Unlock();
			//}
		}
	}
	//return *this;
}
