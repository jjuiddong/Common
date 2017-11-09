
#include "stdafx.h"
#include "line2d.h"

using namespace graphic;


cLine2D::cLine2D()
	: cNode(common::GenerateId(), "line", eNodeType::MODEL)
{
}

cLine2D::cLine2D(cRenderer &renderer
	, const Vector2 &p0 //= Vector2(0, 0)
	, const Vector2 &p1 //= Vector2(100, 100)
	, const float width //= 1.f
	, const int vtxType //= (eVertexType::POSITION_RHW)
	, const cColor color //= cColor::BLACK
)
	: cNode(common::GenerateId(), "line", eNodeType::MODEL)
{
	m_shape.Create2(renderer, vtxType | eVertexType::COLOR, color);
	SetLine(p0, p1, width);
	m_color = color;
}


bool cLine2D::Create(cRenderer &renderer
	, const Vector2 &p0 //= Vector2(0, 0)
	, const Vector2 &p1 //= Vector2(1, 1)
	, const float width //= 1.f
	, const int vtxType //=(eVertexType::POSITION_RHW)
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
	const float halfWidth = m_transform.scale.x * 2 / vp.Width;
	const float halfHeight = m_transform.scale.y * 2 / vp.Height;

	Transform tfm = m_transform;
	tfm.pos.x = -1 + halfWidth + (m_transform.pos.x * 2 / vp.Width);
	tfm.pos.y = 1 - halfHeight - (m_transform.pos.y * 2 / vp.Height);
	tfm.scale *= Vector3(2.f / vp.Width, 2.f / vp.Height, 1);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_boundingBox.GetTransformXM() * parentTm * tfm.GetMatrixXM());
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	const Vector4 color = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(color.x, color.y, color.z, color.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	m_shape.Render(renderer);
	return true;
}


void cLine2D::SetLine(const Vector2 &p0, const Vector2 &p1, const float width)
{
	m_boundingBox.SetLineBoundingBox(Vector3(p0.x, p0.y,0)
		, Vector3(p1.x, p1.y,0), width);
}


void cLine2D::SetColor(const cColor &color)
{
	assert(0);
}

