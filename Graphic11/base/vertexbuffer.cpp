
#include "stdafx.h"
#include "vertexbuffer.h"

using namespace graphic;


cVertexBuffer::cVertexBuffer() 
	: m_sizeOfVertex(0)
	, m_vertexCount(0)
	, m_vtxBuff(NULL)
	//,	m_pVtxDecl(NULL)
	, m_isManagedPool(true)
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
	bd.CPUAccessFlags = (usage== D3D11_USAGE_DEFAULT)? 0 : D3D11_CPU_ACCESS_WRITE;

	//D3D11_SUBRESOURCE_DATA InitData;
	//ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = vertices;

	if (FAILED(renderer.GetDevice()->CreateBuffer(&bd, NULL, &m_vtxBuff)))
	{
		return false;
	}

	m_vertexCount = vertexCount;
	m_sizeOfVertex = sizeofVertex;
	m_isManagedPool = true;
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
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	if (FAILED(renderer.GetDevice()->CreateBuffer(&bd, &InitData, &m_vtxBuff)))
	{
		return false;
	}

	m_vertexCount = vertexCount;
	m_sizeOfVertex = sizeofVertex;
	m_isManagedPool = true;
	return true;
}


// Video Memory 에 버텍스 버퍼를 생성한다.
// 웬만하면 사용하지 말자. 잘쓰려면 복잡해지고, 잘 쓰더라도 효과가 거의 없다.
bool cVertexBuffer::CreateVMem(cRenderer &renderer, const int vertexCount, const int sizeofVertex)
{
	//SAFE_RELEASE(m_vtxBuff);

	//if (FAILED(renderer.GetDevice()->CreateVertexBuffer( vertexCount*sizeofVertex,
	//	D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
	//	fvf,
	//	D3DPOOL_DEFAULT, &m_vtxBuff, NULL)))
	//{
	//	return false;
	//}

	m_vertexCount = vertexCount;
	m_sizeOfVertex = sizeofVertex;
	m_isManagedPool = false;
	return true;
}


// 버텍스 버퍼 생성.
//bool cVertexBuffer::Create(cRenderer &renderer, const int vertexCount, const int sizeofVertex
//	, const cVertexDeclaration *decl)
//{
//	//SAFE_RELEASE(m_vtxBuff);
//	//SAFE_RELEASE(m_pVtxDecl);
//
//	//if (FAILED(renderer.GetDevice()->CreateVertexDeclaration(&decl.GetDecl()[0], &m_pVtxDecl)))
//	//{
//	//	return false; //Failed to create vertex declaration.
//	//}
//
//	//if (FAILED(renderer.GetDevice()->CreateVertexBuffer( vertexCount*sizeofVertex,
//	//	D3DUSAGE_WRITEONLY, 
//	//	0,
//	//	D3DPOOL_MANAGED, &m_vtxBuff, NULL)))
//	//{
//	//	return false;
//	//}
//
//	m_vtxDecl = decl;
//	m_vertexCount = vertexCount;
//	m_sizeOfVertex = sizeofVertex;
//	m_isManagedPool = true;
//	return true;
//}


void* cVertexBuffer::Lock(cRenderer &renderer)
{
	D3D11_MAPPED_SUBRESOURCE res;
	ZeroMemory(&res, sizeof(res));
	HRESULT hr = renderer.GetDevContext()->Map(m_vtxBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	if (FAILED(hr))
		return NULL;
	return res.pData;
}


// idx : 몇 번째 버텍스 부터 Lock 할지를 가르킨다.
// size: 몇 개의 버텍스를 Lock 할지를 가르킨다.
void* cVertexBuffer::LockDiscard(const int idx, const int size) // idx=0, size=0
{
	//RETV(!m_vtxBuff, NULL);
	//RETV(m_isManagedPool, NULL);

	//void *vertices = NULL;
	//if (FAILED(m_vtxBuff->Lock(idx*m_sizeOfVertex, size*m_sizeOfVertex, 
	//	(void**)&vertices, D3DLOCK_DISCARD)))
	//	return NULL;

	//return vertices;
	return NULL;
}


// idx : 몇 번째 버텍스 부터 Lock 할지를 가르킨다.
// size: 몇 개의 버텍스를 Lock 할지를 가르킨다.
void* cVertexBuffer::LockNooverwrite(const int idx, const int size) // idx=0, size=0
{
	//RETV(!m_vtxBuff, NULL);
	//RETV(m_isManagedPool, NULL);

	//void *vertices = NULL;
	//if (FAILED(m_vtxBuff->Lock(idx*m_sizeOfVertex, size*m_sizeOfVertex, 
	//	(void**)&vertices, D3DLOCK_NOOVERWRITE)))
	//	return NULL;

	//return vertices;
	return NULL;
}


void cVertexBuffer::Unlock(cRenderer &renderer)
{
	renderer.GetDevContext()->Unmap(m_vtxBuff, 0);
}


void cVertexBuffer::Bind(cRenderer &renderer) const
{
	const UINT offset = 0;
	const UINT stride = (UINT)m_sizeOfVertex;
	renderer.GetDevContext()->IASetVertexBuffers(0, 1, &m_vtxBuff, &stride, &offset);

	//if (m_pVtxDecl)
	//{
		//renderer.GetDevice()->SetFVF( 0 );
		//renderer.GetDevice()->SetVertexDeclaration(m_pVtxDecl);
	//}
	//else
	//{
		//renderer.GetDevice()->SetFVF(m_fvf);
	//}

	//renderer.GetDevice()->SetStreamSource(0, m_vtxBuff, 0, m_sizeOfVertex);
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
	m_isManagedPool = true;
	SAFE_RELEASE(m_vtxBuff);
	//SAFE_RELEASE(m_pVtxDecl);
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
