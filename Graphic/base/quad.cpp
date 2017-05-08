
#include "stdafx.h"
#include "quad.h"


using namespace graphic;


cQuad::cQuad()
	: m_texture(NULL)
{
	m_material.InitWhite();
	//m_material.InitRed();
}

cQuad::~cQuad()
{
}


// 쿼드를 초기화 한다.
// width, height : 쿼드 크기
// pos : 쿼드 위치
bool cQuad::Create(cRenderer &renderer, const float width, const float height,
	const Vector3 &pos 
	, const string &textureFileName // = " "
	, const bool isSizePow2 // = true
	, const bool isVert //= true
	)
{
	if (m_vtxBuff.GetVertexCount() <= 0)
	{
		m_vtxBuff.Create(renderer, 4, sizeof(sVertexTex), sVertexTex::FVF);
	}

	sVertexTex *vertices = (sVertexTex*)m_vtxBuff.Lock();
	RETV(!vertices, false);

	if (isVert)
	{
		vertices[ 0].p = Vector3(-width/2.f, height/2.f, 0);
		vertices[ 1].p = Vector3(width/2.f, height/2.f, 0);
		vertices[ 2].p = Vector3(-width/2.f,-height/2.f, 0);
		vertices[ 3].p = Vector3(width/2.f,- height/2.f, 0);
	}
	else
	{
		vertices[0].p = Vector3(-width / 2.f, 0, height / 2.f);
		vertices[1].p = Vector3(width / 2.f, 0, height / 2.f);
		vertices[2].p = Vector3(-width / 2.f, 0, -height / 2.f);
		vertices[3].p = Vector3(width / 2.f, 0, -height / 2.f);
	}

	//vertices[ 0].n = Vector3(0, 0, -1);
	//vertices[ 1].n = Vector3(0, 0, -1);
	//vertices[ 2].n = Vector3(0, 0, -1);
	//vertices[ 3].n = Vector3(0, 0, -1);

	vertices[ 0].u = 0;
	vertices[ 0].v = 0;
	vertices[ 1].u = 1;
	vertices[ 1].v = 0;
	vertices[ 2].u = 0;
	vertices[ 2].v = 1;
	vertices[ 3].u = 1;
	vertices[ 3].v = 1;

	m_vtxBuff.Unlock();

	if (!textureFileName.empty())
		m_texture = cResourceManager::Get()->LoadTexture(renderer, textureFileName, isSizePow2);

	m_tm.SetTranslate(pos);

	return true;
}


void cQuad::Render(cRenderer &renderer, const Matrix44 &tm)
// tm = Matrix44::Identity
{
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&(m_tm * tm));
	m_material.Bind(renderer);
	if (m_texture)
		m_texture->Bind(renderer, 0);

	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	// AlphaBlending
	//renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//renderer.GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//renderer.GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//renderer.GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA);
	//renderer.GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);

	m_vtxBuff.RenderTriangleStrip(renderer);
	//renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}


void cQuad::RenderAlpha(cRenderer &renderer, const Matrix44 &tm)
// tm = Matrix44::Identity
{
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&(m_tm * tm));
	m_material.Bind(renderer);
	if (m_texture)
		m_texture->Bind(renderer, 0);

	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	
	// AlphaBlending
	renderer.SetCullMode(D3DCULL_CCW);
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	renderer.GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	renderer.GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_vtxBuff.RenderTriangleStrip(renderer);
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}


void cQuad::RenderFactor(cRenderer &renderer, const Matrix44 &tm)
// tm = Matrix44::Identity
{
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&(m_tm * tm));
	m_material.Bind(renderer);
	if (m_texture)
		m_texture->Bind(renderer, 0);

	DWORD op, arg1;
	renderer.GetDevice()->GetTextureStageState(0, D3DTSS_COLOROP, &op);
	renderer.GetDevice()->GetTextureStageState(0, D3DTSS_COLORARG1, &arg1);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

	m_vtxBuff.RenderTriangleStrip(renderer);

	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, op);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, arg1);
}


void cQuad::RenderLine(cRenderer &renderer)
{
	DWORD flag;
	renderer.GetDevice()->GetRenderState(D3DRS_FILLMODE, &flag);
	renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	Render(renderer);
	renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag);
}


void cQuad::RenderShader(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(!m_shader);

	const Matrix44 transform = m_tm * tm;

	const int pass = m_shader->Begin();
	for (int i = 0; i < pass; ++i)
	{
		m_shader->BeginPass(i);
		m_shader->SetMatrix("g_mWorld", transform);
		if (m_texture)
			m_texture->Bind(*m_shader, "g_colorMapTexture");
		else
			m_shader->SetTexture("g_colorMapTexture", NULL);

		m_shader->CommitChanges();
		m_vtxBuff.RenderTriangleStrip(renderer);
		m_shader->EndPass();
	}
	m_shader->End();
}
