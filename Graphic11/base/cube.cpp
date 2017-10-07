
#include "stdafx.h"
#include "cube.h"

using namespace graphic;


cCube::cCube()
	: cNode2(common::GenerateId(), "cube", eNodeType::MODEL)
{
	m_renderFlags |= eRenderFlag::SHADOW;
}

cCube::cCube(cRenderer &renderer
	, const cBoundingBox &bbox
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
	, const cColor &color //= cColor::WHITE
)
	: cNode2(common::GenerateId(), "cube", eNodeType::MODEL)
{
	Create(renderer, bbox, vtxType, color);
}


bool cCube::Create(cRenderer &renderer
	, const cBoundingBox &bbox
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
	, const cColor &color //= Color::WHITE
)
{
	Create(renderer, vtxType, color);
	SetCube(bbox);
	CalcBoundingSphere();
	return true;
}


bool cCube::Create(
	cRenderer &renderer
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
	, const cColor &color //= Color::WHITE
)
{
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0, 0, 0, 1));
	m_shape.Create2(renderer, vtxType, color);
	CalcBoundingSphere();
	return true;
}


void cCube::SetCube(const cBoundingBox &bbox)
{
	m_transform.scale = bbox.GetDimension() / 2.f;
	m_transform.pos = bbox.Center();
	//m_boundingBox = bbox;
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
	//sVertexNormDiffuse *vbuff = (sVertexNormDiffuse*)m_vtxBuff.Lock();
	//for (int i=0; i < m_vtxBuff.GetVertexCount(); ++i)
	//	vbuff[ i].c = color;
	//m_vtxBuff.Unlock();
}


bool cCube::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	//renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_boundingBox.GetTransformXM() * tm);
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM() * tm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbMaterial.Update(renderer, 2);

	if ((m_shape.m_vtxType & eVertexType::TEXTURE) && m_texture)
		m_texture->Bind(renderer, 0);

	m_shape.Render(renderer);

	__super::Render(renderer, tm, flags);
	return true;
}

