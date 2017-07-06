
#include "stdafx.h"
#include "quad2d.h"


using namespace graphic;

cQuad2D::cQuad2D()
	: m_texture(NULL)
{
	m_material.InitWhite();
}

cQuad2D::~cQuad2D()
{
}


// 쿼드를 초기화 한다.
// width, height : 쿼드 크기
// pos : 쿼드 위치
bool cQuad2D::Create(cRenderer &renderer
	, const float x, const float y
	, const float width, const float height
	, const string &textureFileName // = " "
	, const bool isSizePow2 // = true
)
{
	if (m_vtxBuff.GetVertexCount() <= 0)
		m_vtxBuff.Create(renderer, 4, sizeof(sVertexTexRhw), sVertexTexRhw::FVF);

	SetPosition(x, y, width, height);

	if (!textureFileName.empty())
		m_texture = cResourceManager::Get()->LoadTexture(renderer, textureFileName, isSizePow2);

	return true;
}


void cQuad2D::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	m_material.Bind(renderer);
	if (m_texture)
		m_texture->Bind(renderer, 0);

	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	// AlphaBlending
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	renderer.GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	renderer.GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_vtxBuff.RenderTriangleStrip(renderer);
}


void cQuad2D::SetPosition(const float x, const float y
	, const float width, const float height
)
{
	sVertexTexRhw *vertices = (sVertexTexRhw*)m_vtxBuff.Lock();
	RET(!vertices);

	vertices[0].p = Vector4(x, y, 0, 1);
	vertices[1].p = Vector4(x + width, y, 0, 1);
	vertices[2].p = Vector4(x, y + height, 0, 1);
	vertices[3].p = Vector4(x + width, y + height, 0, 1);

	vertices[0].u = 0;
	vertices[0].v = 0;
	vertices[1].u = 1;
	vertices[1].v = 0;
	vertices[2].u = 0;
	vertices[2].v = 1;
	vertices[3].u = 1;
	vertices[3].v = 1;

	m_vtxBuff.Unlock();
}
