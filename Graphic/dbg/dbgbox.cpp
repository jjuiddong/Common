
#include "stdafx.h"
#include "dbgbox.h"

using namespace graphic;


cDbgBox::cDbgBox()
{
}

cDbgBox::cDbgBox(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax)
{
	SetBox(renderer, vMin, vMax);
}


void cDbgBox::InitBox(cRenderer &renderer)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return;

	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	Vector3 vertices[8] = {
		Vector3(-1,1,-1), Vector3(1,1,-1), Vector3(-1,-1,-1), Vector3(1,-1,-1),
		Vector3(-1,1, 1), Vector3(1,1, 1), Vector3(-1,-1,1), Vector3(1,-1,1),
	};

	InitBox(renderer, vertices);
}


void cDbgBox::InitBox(cRenderer &renderer, Vector3 vertices[8])
{
	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	// vertices

	WORD indices[36] = {
		// front
		0, 3, 2,
		0 ,1, 3,
		// back
		5, 6, 7,
		5, 4, 6,
		// top
		4, 1, 0,
		4, 5, 1,
		// bottom
		2, 7, 6,
		2, 3, 7,
		// left
		4, 2, 6,
		4, 0, 2,
		// right
		1, 7, 3,
		1, 5, 7,
	};

	if (m_vtxBuff.GetVertexCount() <= 0)
	{
		m_vtxBuff.Create(renderer, 36, sizeof(sVertexDiffuse), sVertexDiffuse::FVF);
		m_idxBuff.Create(renderer, 12);
	}

	if (sVertexDiffuse *vbuff = (sVertexDiffuse*)m_vtxBuff.Lock())
	{
		const DWORD color = D3DCOLOR_XRGB(200, 200, 200);
		for (int i = 0; i < 36; ++i)
		{
			vbuff[i].p = vertices[indices[i]];
			vbuff[i].c = color;
		}
		m_vtxBuff.Unlock();
	}

	if (WORD *p = (WORD*)m_idxBuff.Lock())
	{
		for (int i = 0; i < 36; ++i)
			*p++ = i;
		m_idxBuff.Unlock();
	}

	m_min = Vector3(-1, -1, -1);
	m_max = Vector3(1, 1, 1);
}


void cDbgBox::SetBox(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax)
{
	if (m_vtxBuff.GetVertexCount() <= 0)
		InitBox(renderer);

	const Vector3 center = (vMin + vMax) / 2.f;
	const Vector3 v1 = vMin - vMax;
	const Vector3 v2 = m_max - m_min;
	Vector3 scale(abs(v1.x) / 2, abs(v1.y) / 2, abs(v1.z) / 2);

	Matrix44 S;
	S.SetScale(scale);
	Matrix44 T;
	T.SetTranslate(center);
	Matrix44 tm = S * T;

	m_tm = tm;
	m_min = vMin;
	m_max = vMax;
}


void cDbgBox::SetColor(DWORD color)
{
	sVertexDiffuse *vbuff = (sVertexDiffuse*)m_vtxBuff.Lock();
	for (int i = 0; i < m_vtxBuff.GetVertexCount(); ++i)
		vbuff[i].c = color;
	m_vtxBuff.Unlock();
}


void cDbgBox::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(m_vtxBuff.GetVertexCount() <= 0);

	DWORD cullMode;
	DWORD fillMode;
	DWORD lightMode;
	renderer.GetDevice()->GetRenderState(D3DRS_CULLMODE, &cullMode);
	renderer.GetDevice()->GetRenderState(D3DRS_FILLMODE, &fillMode);
	renderer.GetDevice()->GetRenderState(D3DRS_LIGHTING, &lightMode);

	renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, FALSE);
	renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	renderer.GetDevice()->SetTexture(0, NULL);

	Matrix44 mat = m_tm * tm;
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&mat);
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		m_vtxBuff.GetVertexCount(), 0, 12);

	renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, cullMode);
	renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, fillMode);
	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lightMode);
}

