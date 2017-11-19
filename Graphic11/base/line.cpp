
#include "stdafx.h"
#include "line.h"

using namespace graphic;


cLine::cLine() 
	: cNode(common::GenerateId(), "line", eNodeType::MODEL)
{
	m_subType = eSubType::LINE;
}

cLine::cLine(cRenderer &renderer
	, const Vector3 &p0 //= Vector3(0, 0, 0)
	, const Vector3 &p1 //= Vector3(1, 1, 1)
	, const float width //= 1.f
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL)
	, const cColor color //= cColor::BLACK
)
	: cNode(common::GenerateId(), "line", eNodeType::MODEL)
{
	m_subType = eSubType::LINE;
	m_shape.Create2(renderer, vtxType, color);
	SetLine(p0, p1, width);
	m_color = color;
}


bool cLine::Create(cRenderer &renderer
	, const Vector3 &p0 //= Vector3(0, 0, 0)
	, const Vector3 &p1 //= Vector3(1, 1, 1)
	, const float width //= 1.f
	, const int vtxType //=(eVertexType::POSITION | eVertexType::NORMAL)
	, const cColor color //= cColor::BLACK
)
{
	m_shape.Create2(renderer, vtxType, color);
	SetLine(p0, p1, width);
	m_color = color;
	return true;
}


bool cLine::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(transform);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	const Vector4 color = m_color.GetColor();
	Vector4 ambient = m_color.GetColor() * 0.3f;
	ambient.w = m_color.GetColor().w;
	Vector4 diffuse = m_color.GetColor() * 0.7f;
	diffuse.w = m_color.GetColor().w;
	renderer.m_cbMaterial.m_v->ambient = XMVectorSet(ambient.x, ambient.y, ambient.z, ambient.w);
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	renderer.m_cbMaterial.Update(renderer, 2);
	
	m_shape.Render(renderer);

	return true;
}


void cLine::SetLine(const Vector3 &p0, const Vector3 &p1, const float width)
{
	Vector3 v = p1 - p0;
	const float len = v.Length();
	v.Normalize();

	m_transform.pos = (p0 + p1) / 2.f;
	m_transform.scale = Vector3(width, width, len / 2.f);
	Quaternion q(Vector3(0, 0, 1), v);
	m_transform.rot = q;

	Transform tfm = m_transform;
	tfm.pos = Vector3(0, 0, 0);
	m_boundingBox.SetBoundingBox(tfm);
	CalcBoundingSphere();
}


void cLine::SetColor(const cColor &color)
{
	m_color = color;
}

