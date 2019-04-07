
#include "stdafx.h"
#include "slidelist.h"

using namespace graphic;


cSlideList::cSlideList()
	: cNode(common::GenerateId(), "slidelist", eNodeType::MODEL)
	, m_barSize(0.5f)
	, m_width(0.5f)
	, m_isAnimation(false)
	, m_aniVelocity(1.f)
{
}

cSlideList::~cSlideList()
{
	Clear();
}


bool cSlideList::Create(cRenderer &renderer
	, const int reserveSlideCount // = 10
	, const cColor &c0 // = cColor::BLACK
	, const cColor &c1 // = cColor::WHITE
	, const float barSize // = 0.5f
	, const float width // = 0.5f
)
{
	const int vtxType = (eVertexType::POSITION | eVertexType::COLOR);
	m_vtxType = vtxType;
	m_barSize = barSize;
	m_width = width;

	const cColor colors1[8] = { c0, c0, c0, c0, c0, c0, c0, c0 };
	const cColor colors2[8] = { c1, c1, c1, c1, c1, c1, c1, c1 };
	m_shape[0].Create2(renderer, colors1, vtxType);
	m_shape[1].Create2(renderer, colors2, vtxType);
	m_shape[2].Create2(renderer, colors1, vtxType);
	m_color[0] = c0;
	m_color[1] = c1;

	m_slides.reserve(reserveSlideCount);

	return true;
}


bool cSlideList::Update(cRenderer &renderer, const float deltaSeconds)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);
	RETV(!m_isAnimation, true);

	for (auto &slide : m_slides)
	{
		slide.apos += deltaSeconds * m_aniVelocity;
		if (slide.apos >= slide.len)
			slide.apos = 0.f;
		SetSlideShpae(slide, slide.pos[0], slide.pos[1], m_barSize, m_width, slide.apos/slide.len);
	}
		
	return true;
}


// Instancing Render
bool cSlideList::Render(cRenderer &renderer
	, const XMMATRIX &tm // = XMIdentity
	, const int flags // = 1
)
{
	RETV(m_slides.empty(), true);
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);
	RETV(((m_renderFlags & flags) != flags), false);

	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_vtxType);
	assert(shader);
	shader->SetTechnique("Unlit_Instancing");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	const Vector4 diffuse(1, 1, 1, 1);
	const Vector4 alpha = m_color[1].GetColor();
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(diffuse.x, diffuse.y, diffuse.z, alpha.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	CommonStates states(renderer.GetDevice());
	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
		renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
	else
		renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), NULL, 0xffffffff);

	uint offset = 0;
	const int size = ARRAYSIZE(renderer.m_cbInstancing.m_v->worlds);
	while (offset < m_slides.size())
	{
		uint count = 0;
		for (uint i = offset; i < m_slides.size() && (count < size); ++i, ++count)
			renderer.m_cbInstancing.m_v->worlds[i-offset] = XMMatrixTranspose(m_slides[i].tfm[0].GetMatrixXM());
		renderer.m_cbInstancing.Update(renderer, 3);

		m_shape[0].RenderInstancing(renderer, count);

		count = 0;
		for (uint i = offset; i < m_slides.size() && (count < size); ++i, ++count)
			renderer.m_cbInstancing.m_v->worlds[i-offset] = XMMatrixTranspose(m_slides[i].tfm[1].GetMatrixXM());
		renderer.m_cbInstancing.Update(renderer, 3);
		m_shape[1].RenderInstancing(renderer, count);

		count = 0;
		for (uint i = offset; i < m_slides.size() && (count < size); ++i, ++count)
			renderer.m_cbInstancing.m_v->worlds[i - offset] = XMMatrixTranspose(m_slides[i].tfm[2].GetMatrixXM());
		renderer.m_cbInstancing.Update(renderer, 3);
		m_shape[2].RenderInstancing(renderer, count);

		offset += count;
	}

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	else
		renderer.GetDevContext()->OMSetBlendState(states.Opaque(), NULL, 0xffffffff);

	__super::Render(renderer, tm, flags);
	return true;
}


int cSlideList::Add(const int id, const Vector3 &p0, const Vector3 &p1, const float slidePos)
{
	sSlideInfo slide;
	slide.id = id;
	slide.pos[0] = p0;
	slide.pos[1] = p1;
	slide.spos = slidePos;
	slide.apos = 0.f;
	slide.len = p0.Distance(p1);
	SetSlideShpae(slide, p0, p1, m_barSize, m_width, slidePos);
	m_slides.push_back(slide);
	return (int)m_slides.size();
}


bool cSlideList::Remove(const int id)
{
	common::popvector2(m_slides, { id });
	return true;
}


bool cSlideList::SetSlide(const int id, const Vector3 &p0, const Vector3 &p1, const float slidePos)
{
	auto it = std::find_if( m_slides.begin(), m_slides.end()
		, [&](const auto &v) {return v.id == id; });
	if (m_slides.end() == it)
		return false;

	it->spos = min(1.f, max(0.f, slidePos));
	SetSlideShpae(*it, p0, p1, m_barSize, m_width, slidePos);
	return true;
}


void cSlideList::SetAlpha(const float alpha)
{
	Vector4 color = m_color[1].GetColor();
	color.w = alpha;
	m_color[1].SetColor(color);
}


void cSlideList::SetAnimation(const bool isAni, const bool aniVelocity)
{
	m_isAnimation = isAni;
	m_aniVelocity = aniVelocity;
}


// slidePos : 0 ~ 1 range, p0 -> p1
void cSlideList::SetSlideShpae(sSlideInfo &slide,
	const Vector3 &p0, const Vector3 &p1, const float barSize, const float width, const float slidePos)
{
	Vector3 v = p1 - p0;
	const float len = v.Length();
	v.Normalize();

	const Vector3 pt0 = p0;
	const Vector3 pt1 = p0 + v * max(0.f, len * slidePos - barSize);
	const Vector3 pt2 = p0 + v * (len * slidePos);
	const Vector3 pt3 = p1;
	const float len1 = pt0.Distance(pt1);
	const float len2 = pt1.Distance(pt2);
	const float len3 = pt2.Distance(pt3);
	Quaternion q(Vector3(0, 0, 1), v);

	slide.tfm[0].pos = (pt0 + pt1) / 2.f;
	slide.tfm[0].scale = Vector3(width, width, len1 / 2.f);
	slide.tfm[0].rot = q;

	slide.tfm[1].pos = (pt1 + pt2) / 2.f;
	slide.tfm[1].scale = Vector3(width, width, len2 / 2.f);
	slide.tfm[1].rot = q;

	slide.tfm[2].pos = (pt2 + pt3) / 2.f;
	slide.tfm[2].scale = Vector3(width, width, len3 / 2.f);
	slide.tfm[2].rot = q;
}


void cSlideList::Clear()
{
	m_slides.clear();
}
