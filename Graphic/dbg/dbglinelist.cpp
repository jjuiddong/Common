
#include "stdafx.h"
#include "dbglinelist.h"


using namespace graphic;


cDbgLineList::cDbgLineList()
	: m_lineCount(0)
	, m_color(0)
{
}

cDbgLineList::~cDbgLineList()
{
}


bool cDbgLineList::Create(cRenderer &renderer, const int maxLines
	, const DWORD color //= 0
)
{
	m_lineCount = 0;
	m_vtxBuff.Create(renderer, maxLines*2, sizeof(sVertexDiffuse), sVertexDiffuse::FVF);

	return true;
}


bool cDbgLineList::AddLine(const Vector3 &p0, const Vector3 &p1)
{
	if (m_vtxBuff.GetVertexCount() <= (m_lineCount + 1))
		return false; // full buffer

	if (sVertexDiffuse *p = (sVertexDiffuse*)m_vtxBuff.Lock())
	{
		p[m_lineCount].p = p0;
		p[m_lineCount].c = m_color;
		p[m_lineCount+1].p = p1;
		p[m_lineCount+1].c = m_color;
		
		m_lineCount += 2;
		m_vtxBuff.Unlock();
	}

	return true;
}


void cDbgLineList::SetColor(const DWORD color)
{
	if (sVertexDiffuse *p = (sVertexDiffuse*)m_vtxBuff.Lock())
	{
		for (int i = 0; i < m_lineCount; ++i)
		{
			p->c = color;
			++p;
		}
		m_vtxBuff.Unlock();

		m_color = color;
	}
}


void cDbgLineList::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	DWORD lighting;
	renderer.GetDevice()->GetRenderState(D3DRS_LIGHTING, &lighting);
	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	renderer.GetDevice()->SetTexture(0, NULL);

	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&tm);
	m_vtxBuff.Bind(renderer);
	renderer.GetDevice()->DrawPrimitive(D3DPT_LINELIST, 0, m_lineCount/2);

	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lighting);
}


void cDbgLineList::ClearLines()
{
	m_lineCount = 0;
}
