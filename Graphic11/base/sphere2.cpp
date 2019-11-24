#include "stdafx.h"
#include "sphere2.h"


using namespace graphic;


cSphere2::cSphere2()
	: cNode(common::GenerateId(), "sphere", eNodeType::MODEL)
	, m_radius(0)
	, m_texture(NULL)
{
	m_mtrl.InitWhite();
}

cSphere2::cSphere2(cRenderer &renderer, const float radius, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL)
	, const cColor &color //= cColor::WHITE
)
	: cNode(common::GenerateId(), "sphere", eNodeType::MODEL)
	, m_radius(0)
	, m_texture(NULL)
{
	Create(renderer, radius, stacks, slices, vtxType, color);
}

cSphere2::~cSphere2()
{
}


void cSphere2::Create(cRenderer &renderer, const float radius, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL)
	, const cColor &color //= cColor::WHITE
)
{
	m_mtrl.m_diffuse = color.GetColor();
	m_radius = radius;
	m_vtxType = vtxType;
	m_shape.Create(renderer, radius, stacks, slices, vtxType, false);
}


bool cSphere2::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	renderer.m_cbMaterial = m_mtrl.GetMaterial();
	renderer.m_cbMaterial.Update(renderer, 2);

	if ((m_shape.m_vtxType & eVertexType::TEXTURE0) && m_texture)
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

	__super::Render(renderer, parentTm, flags);
	return true;
}


// ray가 바라보는 텍스쳐 uv 좌표를 계산한다.
Vector2 cSphere2::GetDirectionUV(const Ray &ray) const
{
	float dotH = 0.f;
	const Vector3 hdir = Vector3(ray.dir.x, 0, ray.dir.z).Normal();
	dotH = hdir.DotProduct(Vector3(1, 0, 0));

	float dotV = 0.f;
	const Vector3 vdir = Vector3(ray.dir.x, 0, ray.dir.z).Normal();
	dotV = vdir.DotProduct(ray.dir);

	float u = 0.f;
	{
		const float angle = acos(dotH) / MATH_PI;
		if (Vector3(1, 0, 0).CrossProduct(hdir).y > 0)
		{
			u = angle * 0.5f;
		}
		else
		{
			u = 1.f - (angle * 0.5f);
		}
	}

	float v = 0.f;
	{
		float angle = acos(dotV) / MATH_PI;
		if (ray.dir.y > 0)
		{
			v = 0.5f - angle;
		}
		else
		{
			v = abs(angle) + 0.5f;
		}
	}

	return Vector2(u, v);

}
