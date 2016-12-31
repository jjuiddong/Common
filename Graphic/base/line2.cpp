
#include "stdafx.h"
#include "line2.h"

using namespace graphic;

cLine2::cLine2()
{
	m_material.InitWhite();
}

cLine2::cLine2(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width)
{
	SetLine(renderer, p0, p1, width);
	m_material.InitWhite();
}


void cLine2::Render(cRenderer &renderer, const Matrix44 &tm)//tm = Matrix44::Identity
{
	Matrix44 m = m_tm * tm;

	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&m);
	m_material.Bind(renderer);
	m_vtxBuff.Bind(renderer);
	renderer.GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
}


void cLine2::SetLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width)
{
	m_p0 = p0;
	m_p1 = p1;
	m_width = width;

	InitCube(renderer);

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


void cLine2::InitCube(cRenderer &renderer)
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
	Vector3 normals[6] = {
		Vector3(0,0,-1), // front
		Vector3(0,0,1), // back
		Vector3(0,1,0), // top
		Vector3(0,-1,0), // bottom
		Vector3(-1,0,0), // left
		Vector3(1,0,0) // right
	};

	int normalIndices[36] = {
		// front
		0, 0, 0,
		0 ,0, 0,
		// back
		1, 1, 1,
		1, 1, 1,
		// top
		2, 2, 2,
		2, 2, 2,
		// bottom
		3, 3, 3,
		3, 3, 3,
		// left
		4, 4, 4,
		4, 4, 4,
		// right
		5, 5, 5,
		5, 5, 5,
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

	m_vtxBuff.Create(renderer, 36, sizeof(sVertexNormDiffuse), sVertexNormDiffuse::FVF);
	if (sVertexNormDiffuse *vbuff = (sVertexNormDiffuse*)m_vtxBuff.Lock())
	{
		for (int i = 0; i < 36; ++i)
		{
			vbuff[i].p = vertices[indices[i]];
			vbuff[i].n = normals[normalIndices[i]];
			vbuff[i].c = D3DCOLOR_ARGB(0, 255, 255, 255);
		}
		m_vtxBuff.Unlock();
	}
}
