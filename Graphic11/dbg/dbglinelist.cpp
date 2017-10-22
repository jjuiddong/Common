
#include "stdafx.h"
#include "dbglinelist.h"


using namespace graphic;


cDbgLineList::cDbgLineList()
	: m_color(0)
{
}

cDbgLineList::~cDbgLineList()
{
}


bool cDbgLineList::Create(cRenderer &renderer, const int maxLines
	, const cColor &color //= cColor::BLACK
)
{
	m_color = color;
	m_vtxBuff.Create(renderer, maxLines*2, sizeof(sVertex), D3D11_USAGE_DYNAMIC);
	m_lines.reserve(maxLines);

	return true;
}


bool cDbgLineList::AddLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1)
{
	if ((u_int)m_vtxBuff.GetVertexCount() <= (m_lines.size() + 1))
		return false; // full buffer

	m_lines.push_back({ p0, p1 });
	UpdateBuffer(renderer);
	return true;
}


bool cDbgLineList::AddNextPoint(cRenderer &renderer, const Vector3 &p0)
{
	if ((u_int)m_vtxBuff.GetVertexCount() <= (m_lines.size() + 1))
		return false; // full buffer

	Vector3 p = m_lines.empty() ? p0 : m_lines.back().second;
	m_lines.push_back({ p, p0 });
	UpdateBuffer(renderer);
	return true;
}


void cDbgLineList::UpdateBuffer(cRenderer &renderer)
{
	if (sVertex *p = (sVertex*)m_vtxBuff.Lock(renderer))
	{
		for (auto &pt : m_lines)
		{
			p->p = pt.first;
			++p;
			p->p = pt.second;
			++p;
		}

		m_vtxBuff.Unlock(renderer);
	}
}


void cDbgLineList::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	cShader11 *shader = renderer.m_shaderMgr.FindShader(eVertexType::POSITION);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(XMIdentity);
	renderer.m_cbPerFrame.Update(renderer);

	const Vector4 color = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(color.x, color.y, color.z, color.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	m_vtxBuff.Bind(renderer);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	renderer.GetDevContext()->DrawInstanced(m_lines.size()*2, 1, 0, 0);
}


void cDbgLineList::ClearLines()
{
	m_lines.clear();
}
