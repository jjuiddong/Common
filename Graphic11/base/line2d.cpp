
#include "stdafx.h"
#include "line2d.h"

using namespace graphic;


cLine2D::cLine2D()
	: cNode(common::GenerateId(), "line2d", eNodeType::MODEL)
{
}

cLine2D::cLine2D(cRenderer &renderer
	, const Vector2 &p0 //= Vector2(0, 0)
	, const Vector2 &p1 //= Vector2(100, 100)
	, const float width //= 1.f
	, const int vtxType //= (eVertexType::POSITION_RHW)
	, const cColor color //= cColor::BLACK
)
	: cNode(common::GenerateId(), "line2d", eNodeType::MODEL)
{
	m_shape.Create2(renderer, vtxType | eVertexType::COLOR, color);
	SetLine(p0, p1, width);
	m_color = color;
}


bool cLine2D::Create(cRenderer &renderer
	, const Vector2 &p0 //= Vector2(0, 0)
	, const Vector2 &p1 //= Vector2(1, 1)
	, const float width //= 1.f
	, const int vtxType //= (eVertexType::POSITION_RHW)
	, const cColor color //= cColor::BLACK
)
{
	m_shape.Create2(renderer, vtxType | eVertexType::COLOR, color);
	SetLine(p0, p1, width);
	m_color = color;
	return true;
}


bool cLine2D::Render(cRenderer &renderer
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

	// View * Projection 행렬을 mWorld에 저장한다.
	// 2D 모드에서는 View 행렬이 적용되지 않기 때문에, 
	// 바로 Projection 좌표계로 변환한다.
	// Shader에서는 View * Projection을 적용하지 않는다.
	Matrix44 proj;
	proj.SetProjectionScreen(vp.Width, vp.Height, 0, 1);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(
		m_transform.GetMatrixXM() * proj.GetMatrixXM());
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	const Vector4 color = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(color.x, color.y, color.z, color.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	CommonStates state(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(state.CullNone());
	renderer.GetDevContext()->OMSetDepthStencilState(state.DepthNone(), 0);

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
	{
		renderer.GetDevContext()->OMSetBlendState(state.NonPremultiplied(), NULL, 0xffffffff);
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetBlendState(NULL, NULL, 0xffffffff);
	}
	else
	{
		m_shape.Render(renderer);
	}

	renderer.GetDevContext()->OMSetDepthStencilState(state.DepthDefault(), 0);
	renderer.GetDevContext()->RSSetState(state.CullCounterClockwise());

	return true;
}


void cLine2D::SetLine(const Vector2 &p0, const Vector2 &p1, const float width)
{
	Vector2 v = p1 - p0;
	const float len = v.Length();
	v.Normalize();

	Vector2 center = (p1 + p0) / 2.f;
	m_transform.pos = Vector3(center.x, center.y, 0);
	m_transform.scale = Vector3(width, len/2.f, 1);

	Quaternion q;
	q.SetRotationArc(Vector3(0, 1, 0), Vector3(v.x, v.y, 0), Vector3(0,0,1));
	m_transform.rot = q;
}


void cLine2D::SetColor(const cColor &color)
{
	m_color = color;
}
