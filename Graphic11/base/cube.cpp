
#include "stdafx.h"
#include "cube.h"

using namespace graphic;


cCube::cCube()
	: cNode(common::GenerateId(), "cube", eNodeType::MODEL)
{
	m_renderFlags |= eRenderFlag::SHADOW;
	m_subType = eSubType::CUBE;
}

cCube::cCube(cRenderer &renderer
	, const cBoundingBox &bbox
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= cColor::WHITE
	, const int uvFlag //= 1, 0=Create1, 1=Creat2 (cCubeShape)
)
	: cNode(common::GenerateId(), "cube", eNodeType::MODEL)
{
	m_subType = eSubType::CUBE;
	Create(renderer, bbox, vtxType, color, uvFlag);
}


bool cCube::Create(cRenderer &renderer
	, const cBoundingBox &bbox
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= Color::WHITE
	, const int uvFlag //= 1, 0=Create1, 1=Creat2 (cCubeShape)
)
{
	Create(renderer, vtxType, color, uvFlag);
	SetCube(bbox);
	CalcBoundingSphere();
	return true;
}


bool cCube::Create(
	cRenderer &renderer
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= Color::WHITE
	, const int uvFlag //= 1, 0=Create1, 1=Creat2 (cCubeShape)
)
{
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0, 0, 0, 1));
	if (uvFlag == 0)
		m_shape.Create1(renderer, vtxType, color);
	else
		m_shape.Create2(renderer, vtxType, color);

	CalcBoundingSphere();
	m_color = color;
	return true;
}


void cCube::SetCube(const cBoundingBox &bbox)
{
	m_transform = bbox.GetTransform();
	//m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), m_transform.scale, m_transform.rot);
}


void cCube::SetCube(cRenderer &renderer, const cBoundingBox &bbox)
{
	if (m_shape.m_vtxBuff.GetVertexCount() <= 0)
		Create(renderer, m_shape.m_vtxType);
	SetCube(bbox);
}


void cCube::SetCube(cRenderer &renderer, const cCube &cube)
{
	SetCube(renderer, cube.m_boundingBox);
}


void cCube::SetColor(const cColor &color)
{
	m_color = color;
}


bool cCube::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);
	RETV(((m_renderFlags & flags) != flags), false);

	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM() * tm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	Vector4 ambient = m_color.GetColor();// *0.3f;
	ambient.w = m_color.GetColor().w;
	Vector4 diffuse = m_color.GetColor();// *0.7f;
	diffuse.w = m_color.GetColor().w;
	renderer.m_cbMaterial.m_v->ambient = XMVectorSet(ambient.x, ambient.y, ambient.z, ambient.w);
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	if ((m_shape.m_vtxType & eVertexType::TEXTURE) && m_texture)
		m_texture->Bind(renderer, 0);

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
		renderer.GetDevContext()->OMSetBlendState(state.NonPremultiplied(), NULL, 0xffffffff);
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetBlendState(state.Opaque(), NULL, 0xffffffff);
	}

	__super::Render(renderer, tm, flags);
	return true;
}


cNode* cCube::Clone(cRenderer &renderer) const
{
	cCube *clone = new cCube();
	clone->m_shape.m_vtxBuff.Set(renderer, m_shape.m_vtxBuff);
	clone->m_shape.m_idxBuff.Set(renderer, m_shape.m_idxBuff);
	clone->m_shape.m_vtxType = m_shape.m_vtxType;
	clone->m_texture = m_texture;
	clone->m_boundingBox = m_boundingBox;
	clone->m_transform = m_transform;
	clone->m_color = m_color;
	clone->CalcBoundingSphere();
	return clone;
}
