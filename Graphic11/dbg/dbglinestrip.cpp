
#include "stdafx.h"
#include "dbglinestrip.h"


using namespace graphic;


cDbgLineStrip::cDbgLineStrip()
	: m_color(0)
	, m_maxPointCount(0)
	, m_pointCount(0)
{
}

cDbgLineStrip::~cDbgLineStrip()
{
}


bool cDbgLineStrip::Create(cRenderer &renderer, const int maxPoint
	, const cColor &color //= cColor::BLACK
)
{
	m_color = color;
	m_pointCount = 0;
	m_maxPointCount = maxPoint;
	m_vtxBuff.Create(renderer, maxPoint, sizeof(sVertex), D3D11_USAGE_DYNAMIC);
	m_points.reserve(maxPoint);
	return true;
}


bool cDbgLineStrip::AddPoint(cRenderer &renderer, const Vector3 &pos
	, const bool isUpdateBuffer //= true
)
{
	if ((uint)m_vtxBuff.GetVertexCount() <= (m_points.size() + 1))
		return false; // full buffer

	m_points.push_back(pos);

	if (isUpdateBuffer)
		UpdateBuffer(renderer);

	return true;
}


void cDbgLineStrip::UpdateBuffer(cRenderer &renderer)
{
	if (sVertex *p = (sVertex*)m_vtxBuff.Lock(renderer))
	{
		for (auto &pt : m_points)
		{
			p->p = pt;
			++p;
		}
		m_vtxBuff.Unlock(renderer);
	}
	m_pointCount = (uint)m_points.size();
}


void cDbgLineStrip::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	if (m_pointCount <= 0)
		return;

	cShader11 *shader = renderer.m_shaderMgr.FindShader(eVertexType::POSITION);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tm);
	renderer.m_cbPerFrame.Update(renderer);

	const Vector4 color = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(color.x, color.y, color.z, color.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	m_vtxBuff.Bind(renderer);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	renderer.GetDevContext()->DrawInstanced(m_pointCount, 1, 0, 0);

	// debugging
	++renderer.m_drawCallCount;
#ifdef _DEBUG
	++renderer.m_shadersDrawCall[eVertexType::POSITION];
#endif
}


void cDbgLineStrip::ClearLines()
{
	m_points.clear();
}
