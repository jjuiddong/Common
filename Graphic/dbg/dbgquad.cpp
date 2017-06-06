
#include "stdafx.h"
#include "dbgquad.h"

using namespace graphic;


cDbgQuad::cDbgQuad()
{
}

cDbgQuad::~cDbgQuad()
{
}


bool cDbgQuad::Create(cRenderer &renderer)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return true;

	return m_vtxBuff.Create(renderer, 5, sizeof(sVertexDiffuse), sVertexDiffuse::FVF);
}


void cDbgQuad::SetQuad(Vector3 vertices[4])
{
	if (sVertexDiffuse *p = (sVertexDiffuse*)m_vtxBuff.Lock())
	{
		for (int i = 0; i < 5; ++i)
		{
			p[i].p = vertices[i%4];
			p[i].c = 0;
		}
		m_vtxBuff.Unlock();
	}
}


void cDbgQuad::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	m_vtxBuff.RenderLineStrip(renderer);
}
