
#include "stdafx.h"
#include "cube3.h"

using namespace graphic;


cCube3::cCube3()
	: m_tex(NULL)
{
}

cCube3::cCube3(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax)
{
	InitCube(renderer);
	SetCube(renderer, vMin, vMax);
}


void cCube3::InitCube(cRenderer &renderer)
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
	Vector2 uv[] = {
		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(0, 1),
		Vector2(1, 1)
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

	int uvIndices[36] = {
		// front
		0, 3, 2,
		0 ,1, 3,
		// back
		0, 3, 2,
		0 ,1, 3,
		// top
		0, 3, 2,
		0 ,1, 3,
		// bottom
		0, 3, 2,
		0 ,1, 3,
		// left
		0, 3, 2,
		0 ,1, 3,
		// right
		0, 3, 2,
		0 ,1, 3,
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

	m_vtxBuff.Create(renderer, 36, sizeof(sVertexNormTex), sVertexNormTex::FVF);
	m_idxBuff.Create(renderer, 12);

	if (sVertexNormTex *vbuff = (sVertexNormTex*)m_vtxBuff.Lock())
	{
		for (int i = 0; i < 36; ++i)
		{
			vbuff[i].p = vertices[indices[i]];
			vbuff[i].n = normals[normalIndices[i]];
			vbuff[i].u = uv[uvIndices[i]].x;
			vbuff[i].v = uv[uvIndices[i]].y;
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

	m_mtrl.InitWhite();
}


void cCube3::SetCube(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax)
{
	if (m_vtxBuff.GetVertexCount() <= 0)
		InitCube(renderer);

	const Vector3 center = (vMin + vMax) / 2.f;
	const Vector3 v1 = vMin - vMax;
	const Vector3 v2 = m_max - m_min;
	Vector3 scale(sqrt(v1.x*v1.x) / sqrt(v2.x*v2.x),
		sqrt(v1.y*v1.y) / sqrt(v2.y*v2.y),
		sqrt(v1.z*v1.z) / sqrt(v2.z*v2.z));

	Matrix44 S;
	S.SetScale(scale);
	Matrix44 T;
	T.SetTranslate(center);
	Matrix44 tm = S * T;

	sVertexNormTex *vbuff = (sVertexNormTex*)m_vtxBuff.Lock();
	for (int i = 0; i < m_vtxBuff.GetVertexCount(); ++i)
		vbuff[i].p *= tm;
	m_vtxBuff.Unlock();

	m_min = vMin;
	m_max = vMax;
}


void cCube3::SetCube(cRenderer &renderer, const cCube3 &cube)
{
	SetCube(renderer, cube.m_min, cube.m_max);
	m_tm = cube.m_tm;
}


void cCube3::Render(cRenderer &renderer, const Matrix44 &tm)
{
	renderer.GetDevice()->SetTexture(0, NULL);

	Matrix44 mat = m_tm * tm;
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&mat);
	m_mtrl.Bind(renderer);
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		m_vtxBuff.GetVertexCount(), 0, 12);
}


void cCube3::RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &tm)
{
	shader.SetMatrix("g_mWorld", m_tm*tm);
	if (m_tex)
		m_tex->Bind(shader, "g_colorMapTexture");
	
	m_mtrl.Bind(shader);

	const int passCount = shader.Begin();
	for (int i = 0; i < passCount; ++i)
	{
		shader.BeginPass(0);
		shader.CommitChanges();

		m_vtxBuff.Bind(renderer);
		m_idxBuff.Bind(renderer);
		renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
			m_vtxBuff.GetVertexCount(), 0, 12);

		shader.EndPass();
	}
	shader.End();
}
