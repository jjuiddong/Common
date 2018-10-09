
#include "stdafx.h"
#include "dbgline.h"

using namespace graphic;


cDbgLine::cDbgLine()
	: m_isSolid(false)
{
}

cDbgLine::cDbgLine(cRenderer &renderer
	, const Vector3 &p0 //= Vector3(0, 0, 0)
	, const Vector3 &p1 //= Vector3(1, 1, 1)
	, const float width //= 1.f
	, const cColor color //= cColor::BLACK
)
{
	m_shape.Create2(renderer, eVertexType::POSITION);
	SetLine(p0, p1, width);
	m_color = color;
}


bool cDbgLine::Create(cRenderer &renderer
	, const Vector3 &p0 //= Vector3(0, 0, 0)
	, const Vector3 &p1 //= Vector3(1, 1, 1)
	, const float width //= 1.f
	, const cColor color //= cColor::BLACK
)
{
	m_shape.Create2(renderer, eVertexType::POSITION);
	SetLine(p0, p1, width);
	m_color = color;
	return true;
}


bool cDbgLine::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(transform);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	const Vector4 color = m_color.GetColor();
	Vector4 ambient = m_color.GetColor();
	Vector4 diffuse = m_color.GetColor();
	renderer.m_cbMaterial.m_v->ambient = XMVectorSet(ambient.x, ambient.y, ambient.z, ambient.w);
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	CommonStates states(renderer.GetDevice());
	if (m_isSolid)
	{
		renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	}
	else
	{
		ID3D11RasterizerState *oldState = NULL;
		renderer.GetDevContext()->RSGetState(&oldState);
		renderer.GetDevContext()->RSSetState(states.Wireframe());
		m_shape.Render(renderer);
		renderer.GetDevContext()->RSSetState(oldState);
	}

	return true;
}



void cDbgLine::SetLine(const Vector3 &p0, const Vector3 &p1, const float width)
{
	Vector3 v = p1 - p0;
	const float len = v.Length();
	v.Normalize();

	m_transform.pos = (p0 + p1) / 2.f;
	m_transform.scale = Vector3(width, width, len / 2.f);
	Quaternion q(Vector3(0, 0, 1), v);
	m_transform.rot = q;
}


void cDbgLine::SetColor(const cColor &color)
{
	m_color = color;
}

