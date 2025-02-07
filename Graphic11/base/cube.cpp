
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
	m_vtxType = vtxType;
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0, 0, 0, 1));
	if (uvFlag == 0)
		m_shape.Create1(renderer, vtxType, color);
	else
		m_shape.Create2(renderer, vtxType, color);

	CalcBoundingSphere();
	m_color = color;
	return true;
}


void cCube::SetCube(const Transform &tfm)
{
	m_transform = tfm;
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


// render cube
// flags: composite of eRenderFlag 
bool cCube::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);
	//RETV(((m_renderFlags & flags) != flags), false);

	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM() * tm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	Vector4 ambient = m_color.GetColor() * 0.3f;
	Vector4 diffuse = m_color.GetColor();
	renderer.m_cbMaterial.m_v->ambient = XMVectorSet(ambient.x, ambient.y, ambient.z, diffuse.w);
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	if ((m_shape.m_vtxType & eVertexType::TEXTURE0) && m_texture)
		m_texture->Bind(renderer, 0);

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
	{
		renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.NonPremultiplied(), 0, 0xffffffff);
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	}
	else if ((flags & eRenderFlag::WIREFRAME) || IsRenderFlag(eRenderFlag::WIREFRAME))
	{
		renderer.m_cbMaterial.m_v->ambient = XMVectorSet(0, 0, 0, 0);
		renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(0, 0, 0, 0);
		renderer.m_cbMaterial.Update(renderer, 2);

		ID3D11RasterizerState* oldState = NULL;
		renderer.GetDevContext()->RSGetState(&oldState);
		renderer.GetDevContext()->RSSetState(renderer.m_renderState.Wireframe());
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
		renderer.GetDevContext()->RSSetState(oldState);
	}
	else
	{
		renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.NonPremultiplied(), 0, 0xffffffff);
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.Opaque(), NULL, 0xffffffff);
	}

	__super::Render(renderer, tm, flags);
	return true;
}


// instancing render
bool cCube::RenderInstancing(cRenderer& renderer
	, const int count
	, const Matrix44* transforms
	, const XMMATRIX& parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11* shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	const Str32 technique = m_techniqueName + "_Instancing";
	shader->SetTechnique(technique.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;
	for (int i = 0; i < count; ++i)
	{
		const XMMATRIX tm = transforms[i].GetMatrixXM();
		renderer.m_cbInstancing.m_v->worlds[i] = XMMatrixTranspose(tm * transform);
	}

	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbInstancing.Update(renderer, 3);

	const Vector4 color = m_color.GetColor();
	Vector4 ambient = m_color.GetColor();
	Vector4 diffuse = m_color.GetColor();
	renderer.m_cbMaterial.m_v->ambient = XMVectorSet(ambient.x, ambient.y, ambient.z, ambient.w);
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	m_shape.RenderInstancing(renderer, count);

	return __super::RenderInstancing(renderer, count, transforms, parentTm, flags);
}


// instancing render
bool cCube::RenderInstancing2(cRenderer& renderer
	, const int count
	, const Matrix44* transforms
	, const Vector3* colors
	, const XMMATRIX& parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11* shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	const Str32 technique = m_techniqueName + "_Instancing2";
	shader->SetTechnique(technique.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;
	for (int i = 0; i < count; ++i)
	{
		const XMMATRIX tm = transforms[i].GetMatrixXM();
		const XMVECTOR col = colors[i].GetVectorXM();
		renderer.m_cbInstancing.m_v->worlds[i] = XMMatrixTranspose(tm * transform);
		renderer.m_cbInstancing.m_v->diffuses[i] = col;
	}

	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbInstancing.Update(renderer, 3);

	renderer.m_cbMaterial.m_v->ambient = XMVectorSet(1,1,1,1);
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(1,1,1,1);
	renderer.m_cbMaterial.Update(renderer, 2);

	m_shape.RenderInstancing(renderer, count);

	return __super::RenderInstancing(renderer, count, transforms, parentTm, flags);
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
