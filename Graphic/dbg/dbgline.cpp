
#include "stdafx.h"
#include "dbgline.h"

using namespace graphic;

cDbgLine::cDbgLine()
{
}

cDbgLine::cDbgLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width)
{
	SetLine(renderer, p0, p1, width);
}


void cDbgLine::Render(cRenderer &renderer, const Matrix44 &tm)
//tm = Matrix44::Identity
{
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

	Matrix44 m = m_tm * tm;

	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&m);
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		m_vtxBuff.GetVertexCount(), 0, 12);

	renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, cullMode);
	renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, fillMode);
	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lightMode);
}


void cDbgLine::SetLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width
	, const D3DCOLOR color) //color=0
{
	InitCube(renderer, color);
	SetLine(p0, p1, width, color);
}


void cDbgLine::SetLine(const Vector3 &p0, const Vector3 &p1, const float width
	, const D3DCOLOR color //= 0
)
{
	m_p0 = p0;
	m_p1 = p1;
	m_width = width;

	Vector3 v = p1 - p0;
	const float len = v.Length();
	v.Normalize();

	Matrix44 matS;
	matS.SetScale(Vector3(width, width, len / 2.f));

	Matrix44 matT;
	matT.SetTranslate((p0 + p1) / 2.f);

	Quaternion q(Vector3(0, 0, 1), v);
	m_tm = matS * q.GetMatrix() * matT;
}


void cDbgLine::InitCube(cRenderer &renderer, const D3DCOLOR color)
//color = 0
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
	Vector3 norms[6] = {
		Vector3(0,0,-1), // front
		Vector3(0,0,1),  // back
		Vector3(0,1,0),  // top
		Vector3(0,-1,0), // bottom
		Vector3(-1,0, 0),  // left
		Vector3(1,0, 0),  // right
	};

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

	m_vtxBuff.Create(renderer, 8, sizeof(sVertexDiffuse), sVertexDiffuse::FVF);
	m_idxBuff.Create(renderer, 12);

	sVertexDiffuse *vbuff = (sVertexDiffuse*)m_vtxBuff.Lock();
	WORD *ibuff = (WORD*)m_idxBuff.Lock();

	for (int i = 0; i < 8; ++i)
	{
		vbuff[i].p = vertices[i];
		vbuff[i].c = color;
	}

	for (int i = 0; i < 36; ++i)
		ibuff[i] = indices[i];

	m_vtxBuff.Unlock();
	m_idxBuff.Unlock();
}
