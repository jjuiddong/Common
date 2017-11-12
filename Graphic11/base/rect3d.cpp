
#include "stdafx.h"
#include "rect3d.h"

using namespace graphic;


cRect3D::cRect3D()
	: cNode(common::GenerateId(), "rect3d", eNodeType::VIRTUAL)
	, m_color(cColor::BLACK)
	, m_lineCount(0)
	, m_rectId(0)
{
}

cRect3D::~cRect3D()
{
}


bool cRect3D::Create(cRenderer &renderer
	, const int maxEdgeLineCount //= 32
)
{
	m_vtxBuff.Create(renderer, maxEdgeLineCount * 4, sizeof(sVertex), D3D11_USAGE_DYNAMIC);
	return true;
}


bool cRect3D::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);
	RETV(m_lineCount <= 0, true);

	cShader11 *shader = renderer.m_shaderMgr.FindShader(eVertexType::POSITION);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(parentTm);
	renderer.m_cbPerFrame.Update(renderer);

	const Vector4 color = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(color.x, color.y, color.z, color.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	m_vtxBuff.Bind(renderer);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	renderer.GetDevContext()->DrawInstanced(m_lineCount, 1, 0, 0);
	return true;
}


// Order: lefttop - righttop - rightbottom - leftbottom
bool cRect3D::SetRect(cRenderer &renderer, const Vector3 pos[4])
{
	Vector3 tmp[5];
	tmp[0] = pos[0];
	tmp[1] = pos[1];
	tmp[2] = pos[2];
	tmp[3] = pos[3];
	tmp[4] = pos[0];
	return SetRect(renderer, tmp, 5);
}


bool cRect3D::SetRect(cRenderer &renderer, const vector<Vector3> &lines)
{
	return SetRect(renderer, &lines[0], (int)lines.size());
}


bool cRect3D::SetRect(cRenderer &renderer, const Vector3 lines[], const int size)
{
	const int edgeIdx = size / 4;
	m_pos[0] = lines[0];
	m_pos[1] = lines[(edgeIdx * 1) - 1];
	m_pos[2] = lines[(edgeIdx * 2) - 1];
	m_pos[3] = lines[(edgeIdx * 3) - 1];

	if (sVertex *p = (sVertex*)m_vtxBuff.Lock(renderer))
	{
		for (int i=0; i < size; ++i)
		{
			p->p = lines[i];
			++p;
		}
		m_vtxBuff.Unlock(renderer);

		m_lineCount = size;
		return true;
	}
	else
	{
		m_lineCount = 0;
		return false;
	}
}


bool cRect3D::SetRect(cRenderer &renderer, const cRect3D &rect3D)
{
	if (this != &rect3D)
	{
		m_lineCount = rect3D.m_lineCount;
		m_color = rect3D.m_color;
		memcpy(m_pos, rect3D.m_pos, sizeof(m_pos));
		m_vtxBuff.Set(renderer, rect3D.m_vtxBuff);
	}
	return true;
}
