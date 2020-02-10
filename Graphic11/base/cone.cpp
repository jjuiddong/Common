
#include "stdafx.h"
#include "cone.h"


using namespace graphic;


cCone::cCone()
	: graphic::cNode(common::GenerateId(), "cone", eNodeType::MODEL)
{
}

cCone::~cCone()
{
}


// create cone renderer
bool cCone::Create(cRenderer &renderer, const float radius, const float height
	, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= cColor::WHITE
)
{
	if (!m_shape.Create(renderer, 1.f, 1.f, slices, vtxType, color))
		return false;

	m_radiusX = radius;
	m_radiusZ = radius;
	m_height = height;
	m_color = color;
	m_vtxType = vtxType;
	
	m_transform = Transform(Vector3(0, 0, 0), Vector3(radius,height, radius));

	return true;
}


bool cCone::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbLight.Update(renderer, 1);

	const Vector4 diffuse = m_color.GetColor();
	renderer.m_cbMaterial.m_v->ambient = XMVectorSet(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);

	ID3D11RasterizerState *oldState = NULL;
	renderer.GetDevContext()->RSGetState(&oldState);

	if (flags & eRenderFlag::WIREFRAME)
		renderer.GetDevContext()->RSSetState(renderer.m_renderState.Wireframe());

	renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.NonPremultiplied(), 0, 0xffffffff);
	m_shape.Render(renderer);

	renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.Opaque(), NULL, 0xffffffff);
	renderer.GetDevContext()->RSSetState(oldState);

	__super::Render(renderer, parentTm, flags);
	return true;
}


void cCone::SetPos(const Vector3 &pos)
{
	m_transform.pos = pos;
}


void cCone::SetDimension(const float radius, const float height)
{
	m_radiusX = radius;
	m_radiusZ = radius;
	m_height = height;
	m_transform.scale = Vector3(radius, height, radius);
}


void cCone::SetRadius(const float radius)
{
	m_radiusX = radius;
	m_radiusZ = radius;
	m_transform.scale = Vector3(radius, m_height, radius);
}


void cCone::SetRadiusXZ(const float radiusX, const float radiusZ)
{
	m_radiusX = radiusX;
	m_radiusZ = radiusZ;
	m_transform.scale = Vector3(radiusX, m_height, radiusZ);
}


void cCone::SetHeight(const float height)
{
	m_height = height;
	m_transform.scale = Vector3(m_radiusX, height, m_radiusZ);
}
