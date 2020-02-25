
#include "stdafx.h"
#include "circle2d.h"


using namespace graphic;

cCircle2D::cCircle2D()
	: cNode(common::GenerateId(), "circle2d", eNodeType::MODEL)
	, m_radius(1)
	, m_color(cColor::WHITE)
{
}

cCircle2D::~cCircle2D()
{
}


// 원 생성
bool cCircle2D::Create(cRenderer &renderer, const Vector3 &center
	, const float radius, const int slice
	, const cColor &color //= cColor::BLACK
)
{
	m_shape.Create(renderer, center, radius, slice, eVertexType::POSITION_RHW);

	m_vtxType = m_shape.m_vtxType;
	m_color = color;
	m_radius = radius;
	return true;
}


// 출력.
bool cCircle2D::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
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

	// precompute view, projection matrix
	// because rhw shader no calc view, projection matrix
	Matrix44 proj;
	proj.SetProjectionScreen(vp.Width, vp.Height, 0, 1);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(
		m_transform.GetMatrixXM() * parentTm * proj.GetMatrixXM());
	renderer.m_cbPerFrame.Update(renderer);

	const Vector4 diffuse = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = diffuse.GetVectorXM();
	renderer.m_cbMaterial.Update(renderer, 2);

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
	{
		CommonStates states(renderer.GetDevice());
		renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	}
	else
	{
		CommonStates state(renderer.GetDevice());
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
		renderer.GetDevContext()->OMSetDepthStencilState(state.DepthNone(), 0);
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetDepthStencilState(state.DepthDefault(), 0);
	}

	__super::Render(renderer, parentTm, flags);
	return true;
}


// optimize render
// skip shader update, material update
bool cCircle2D::Render2(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	UINT numVp = 1;
	D3D11_VIEWPORT vp;
	renderer.GetDevContext()->RSGetViewports(&numVp, &vp);

	Matrix44 proj;
	proj.SetProjectionScreen(vp.Width, vp.Height, 0, 1);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(
		m_transform.GetMatrixXM() * parentTm * proj.GetMatrixXM());
	renderer.m_cbPerFrame.Update(renderer);

	m_shape.Render(renderer);
	return true;
}


void cCircle2D::SetPos(const Vector2 &pos)
{
	m_transform.pos = Vector3(pos.x, pos.y, 0.f);
}
