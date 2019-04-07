
#include "stdafx.h"
#include "slidebar.h"

using namespace graphic;


cSlideBar::cSlideBar()
	: cNode(common::GenerateId(), "slidebar", eNodeType::MODEL)
	, m_slidePos(0)
	, m_width(0.5f)
	, m_isAnimation(false)
	, m_aniVelocity(1.f)
{
	m_pos[0] = Vector3::Zeroes;
	m_pos[1] = Vector3::Zeroes;
}

cSlideBar::~cSlideBar()
{
}


bool cSlideBar::Create(cRenderer &renderer
	, const Vector3 &p0 // = Vector3::Zeroes
	, const Vector3 &p1 // = Vector3::Zeroes
	, const cColor &c0 // = cColor::BLACK
	, const cColor &c1 // = cColor::WHITE
	, const float width // = 0.5f
	, const float slidePos // = 0.5f;
)
{
	const int vtxType = (eVertexType::POSITION | eVertexType::COLOR);
	m_vtxType = vtxType;

	const cColor colors1[8] = { c0, c0, c0, c0, c1, c1, c1, c1};
	const cColor colors2[8] = { c1, c1, c1, c1, c0, c0, c0, c0 };
	m_shape[0].Create2(renderer, colors1, vtxType);
	m_shape[1].Create2(renderer, colors2, vtxType);
	m_color[0] = c0;
	m_color[1] = c1;

	SetSlideBar(p0, p1, width, slidePos);

	return true;
}


bool cSlideBar::Update(cRenderer &renderer, const float deltaSeconds)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);

	if (m_isAnimation)
	{
		SetSlidePos(m_slidePos + deltaSeconds * m_aniVelocity);
		if (m_slidePos >= 1.f)
			m_slidePos = 0.f;
	}

	__super::Update(renderer, deltaSeconds);
	return true;
}


bool cSlideBar::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);
	RETV(((m_renderFlags & flags) != flags), false);

	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbLight.Update(renderer, 1);

	const Vector4 diffuse(1, 1, 1, 1);
	const Vector4 alpha = m_color[1].GetColor();
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(diffuse.x, diffuse.y, diffuse.z, alpha.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	const XMMATRIX tm0 = m_transform.GetMatrixXM();

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
	{
		CommonStates states(renderer.GetDevice());
		renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);

		renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_tfm[0].GetMatrixXM() * tm0 * tm);
		renderer.m_cbPerFrame.Update(renderer);
		m_shape[0].Render(renderer);

		renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_tfm[1].GetMatrixXM() * tm0 * tm);
		renderer.m_cbPerFrame.Update(renderer);
		m_shape[1].Render(renderer);

		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	}
	else
	{
		CommonStates state(renderer.GetDevice());
		renderer.GetDevContext()->OMSetBlendState(state.NonPremultiplied(), NULL, 0xffffffff);

		renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_tfm[0].GetMatrixXM() * tm);
		renderer.m_cbPerFrame.Update(renderer);
		m_shape[0].Render(renderer);

		renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_tfm[1].GetMatrixXM() * tm);
		renderer.m_cbPerFrame.Update(renderer);
		m_shape[1].Render(renderer);

		renderer.GetDevContext()->OMSetBlendState(state.Opaque(), NULL, 0xffffffff);
	}

	__super::Render(renderer, tm, flags);
	return true;
}


void cSlideBar::SetSlidePos(const float p)
{
	m_slidePos = min(1.f, max(0.f, p));
	SetShape(m_pos[0], m_pos[1], m_width, m_slidePos);
}


// slidePos : 0 ~ 1 range, p0 -> p1
void cSlideBar::SetSlideBar(const Vector3 &p0, const Vector3 &p1, const float width, const float slidePos)
{
	m_slidePos = slidePos;
	m_pos[0] = p0;
	m_pos[1] = p1;
	m_width = width;
	SetShape(p0, p1, width, slidePos);
}


void cSlideBar::SetColor(const cColor &c0, const cColor &c1)
{
	m_color[0] = c0;
	m_color[1] = c1;
}


void cSlideBar::SetAlpha(const float alpha)
{
	Vector4 color = m_color[1].GetColor();
	color.w = alpha;
	m_color[1].SetColor(color);
}


void cSlideBar::SetAnimation(const bool isAni, const bool aniVelocity)
{
	m_isAnimation = isAni;
	m_aniVelocity = aniVelocity;
}


// slidePos : 0 ~ 1 range, p0 -> p1
void cSlideBar::SetShape(const Vector3 &p0, const Vector3 &p1, const float width, const float slidePos)
{
	Vector3 v = p1 - p0;
	const float len = v.Length();
	v.Normalize();

	const Vector3 pt0 = p0;
	const Vector3 pt1 = p0 + v * len * slidePos;
	const Vector3 pt2 = p1;
	const float len1 = pt0.Distance(pt1);
	const float len2 = pt1.Distance(pt2);
	Quaternion q(Vector3(0, 0, 1), v);

	m_tfm[0].pos = (pt0 + pt1) / 2.f;
	m_tfm[0].scale = Vector3(width, width, len1 / 2.f);
	m_tfm[0].rot = q;

	m_tfm[1].pos = (pt1 + pt2) / 2.f;
	m_tfm[1].scale = Vector3(width, width, len2 / 2.f);
	m_tfm[1].rot = q;
}
