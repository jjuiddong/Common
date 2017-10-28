
#include "stdafx.h"
#include "quad2d.h"


using namespace graphic;

cQuad2D::cQuad2D()
	: cNode(common::GenerateId(), "Quad2d", eNodeType::MODEL)
	, m_texture(NULL)
{
}

cQuad2D::~cQuad2D()
{
}


// Äõµå¸¦ ÃÊ±âÈ­ ÇÑ´Ù.
// width, height : Äõµå Å©±â
// pos : Äõµå À§Ä¡
bool cQuad2D::Create(cRenderer &renderer, const float x, const float y, const float width, const float height
	, const char *textureFileName // = " "
)
{
	m_shape.Create(renderer, eVertexType::POSITION_RHW | eVertexType::DIFFUSE | eVertexType::TEXTURE, cColor::WHITE);

	SetPosition(x, y, width, height);

	m_texture = cResourceManager::Get()->LoadTexture(renderer, textureFileName? textureFileName : g_defaultTexture);

	return true;
}


void cQuad2D::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	UINT numVp = 1;
	D3D11_VIEWPORT vp;
	renderer.GetDevContext()->RSGetViewports(&numVp, &vp);

	const float halfWidth = m_transform.scale.x*2 / vp.Width;
	const float halfHeight = m_transform.scale.y*2 / vp.Height;

	Transform tfm = m_transform;
	tfm.pos.x = -1 + halfWidth + (m_transform.pos.x*2 / vp.Width);
	tfm.pos.y = 1 - halfHeight - (m_transform.pos.y * 2 / vp.Height);
	tfm.scale *= Vector3(2.f/ vp.Width, 2.f/ vp.Height, 1);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm.GetMatrixXM());
	renderer.m_cbPerFrame.Update(renderer);

	CommonStates states(renderer.GetDevice());
	//renderer.GetDevContext()->OMSetBlendState(states.Opaque(), 0, 0xffffffff);
	//renderer.GetDevContext()->OMSetBlendState(states.AlphaBlend(), 0, 0xffffffff);
	renderer.GetDevContext()->OMSetDepthStencilState(states.DepthNone(), 0);

	if (m_texture)
		m_texture->Bind(renderer, 0);

	m_shape.Render(renderer);

	//renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);
}


void cQuad2D::SetPosition(const float x, const float y
	, const float width, const float height
)
{
	m_transform.pos = Vector3(x, y, 0);
	m_transform.scale = Vector3(width / 2, height / 2, 1);
}
