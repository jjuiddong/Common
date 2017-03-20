
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

	m_vtxBuff.Create(renderer, 36, sizeof(sVertexNormDiffuseTex), sVertexNormDiffuseTex::FVF);
	m_idxBuff.Create(renderer, 12);

	if (sVertexNormDiffuseTex *vbuff = (sVertexNormDiffuseTex*)m_vtxBuff.Lock())
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



void cCube3::InitCube2(cRenderer &renderer)
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
	Vector3 vertices[24] = {
		Vector3(-1.f, 1.f, -1.f),
		Vector3(1.f, -1.f, -1.f),
		Vector3(-1.f, -1.f, -1.f),
		Vector3(1.f, 1.f, -1.f),
		Vector3(-1.f, -1.f, -1.f),
		Vector3(1.f, -1.f, 1.f),
		Vector3(-1.f, -1.f, 1.f),
		Vector3(1.f, -1.f, -1.f),
		Vector3(-1.f, -1.f, 1.f),
		Vector3(1.f, 1.f, 1.f),
		Vector3(-1.f, 1.f, 1.f),
		Vector3(1.f, -1.f, 1.f),
		Vector3(-1.f, 1.f, 1.f),
		Vector3(1.f, 1.f, -1.f),
		Vector3(-1.f, 1.f, -1.f),
		Vector3(1.f, 1.f, 1.f),
		Vector3(-1.f, -1.f, 1.f),
		Vector3(-1.f, 1.f, -1.f),
		Vector3(-1.f, -1.f, -1.f),
		Vector3(-1.f, 1.f, 1.f),
		Vector3(1.f, -1.f, 1.f),
		Vector3(1.f, 1.f, -1.f),
		Vector3(1.f, 1.f, 1.f),
		Vector3(1.f, -1.f, -1.f),
	};
	Vector3 normals[24] = {
		Vector3(0.f, 0.006981f, -0.999976f),
		Vector3(0.f, 0.006981f, -0.999976f),
		Vector3(0.f, 0.006981f, -0.999976f),
		Vector3(0.f, 0.006981f, -0.999976f),
		Vector3(0.013529f, -0.999884f, -0.006981f),
		Vector3(0.013529f, -0.999884f, -0.006981f),
		Vector3(0.013529f, -0.999884f, -0.006981f),
		Vector3(0.013529f, -0.999884f, -0.006981f),
		Vector3(0.f, -0.006981f, 0.999976f),
		Vector3(0.f, -0.006981f, 0.999976f),
		Vector3(0.f, -0.006981f, 0.999976f),
		Vector3(0.f, -0.006981f, 0.999976f),
		Vector3(-0.013529f, 0.999884f, 0.006981f),
		Vector3(-0.013529f, 0.999884f, 0.006981f),
		Vector3(-0.013529f, 0.999884f, 0.006981f),
		Vector3(-0.013529f, 0.999884f, 0.006981f),
		Vector3(-0.999908f, -0.013529f, -9.4e-05f),
		Vector3(-0.999908f, -0.013529f, -9.4e-05f),
		Vector3(-0.999908f, -0.013529f, -9.4e-05f),
		Vector3(-0.999908f, -0.013529f, -9.4e-05f),
		Vector3(0.999908f, 0.013529f, 9.4e-05f),
		Vector3(0.999908f, 0.013529f, 9.4e-05f),
		Vector3(0.999908f, 0.013529f, 9.4e-05f),
		Vector3(0.999908f, 0.013529f, 9.4e-05f),
	};
	Vector2 uv[24] = {
		Vector2(0.278796f, 0.399099f),
		Vector2(0.008789f, 0.598055f),
		Vector2(0.008789f, 0.399099f),
		Vector2(0.278796f, 0.598055f),
		Vector2(0.987252f, 0.399099f),
		Vector2(0.768028f, 0.598055f),
		Vector2(0.768028f, 0.399099f),
		Vector2(0.987252f, 0.598055f),
		Vector2(0.768028f, 0.399099f),
		Vector2(0.49802f, 0.598055f),
		Vector2(0.49802f, 0.399099f),
		Vector2(0.768028f, 0.598055f),
		Vector2(0.49802f, 0.399099f),
		Vector2(0.278796f, 0.598055f),
		Vector2(0.278796f, 0.399099f),
		Vector2(0.49802f, 0.598055f),
		Vector2(0.49802f, 0.198324f),
		Vector2(0.278796f, 0.399099f),
		Vector2(0.278796f, 0.198324f),
		Vector2(0.49802f, 0.399099f),
		Vector2(0.49802f, 0.79883f),
		Vector2(0.278796f, 0.598055f),
		Vector2(0.49802f, 0.598055f),
		Vector2(0.278796f, 0.79883f),
	};
	WORD indices[36] = {
		0, 1, 2,
		1, 0, 3,
		4, 5, 6,
		5, 4, 7,
		8, 9, 10,
		9, 8, 11,
		12, 13, 14,
		13, 12, 15,
		16, 17, 18,
		17, 16, 19,
		20, 21, 22,
		21, 20, 23,
	};

	m_vtxBuff.Create(renderer, 36, sizeof(sVertexNormDiffuseTex), sVertexNormDiffuseTex::FVF);
	m_idxBuff.Create(renderer, 12);

	if (sVertexNormDiffuseTex *vbuff = (sVertexNormDiffuseTex*)m_vtxBuff.Lock())
	{
		for (int i = 0; i < 36; ++i)
		{
			vbuff[i].p = vertices[indices[i]];
			vbuff[i].n = normals[indices[i]];
			vbuff[i].u = uv[indices[i]].x;
			vbuff[i].v = uv[indices[i]].y;
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
	Vector3 scale(abs(v1.x) / 2, abs(v1.y) / 2, abs(v1.z) / 2);
	//Vector3 scale(sqrt(v1.x*v1.x) / sqrt(v2.x*v2.x),
	//	sqrt(v1.y*v1.y) / sqrt(v2.y*v2.y),
	//	sqrt(v1.z*v1.z) / sqrt(v2.z*v2.z));

	Matrix44 S;
	S.SetScale(scale);
	Matrix44 T;
	T.SetTranslate(center);
	Matrix44 tm = S * T;

	//sVertexNormDiffuseTex *vbuff = (sVertexNormDiffuseTex*)m_vtxBuff.Lock();
	//for (int i = 0; i < m_vtxBuff.GetVertexCount(); ++i)
	//	vbuff[i].p *= tm;
	//m_vtxBuff.Unlock();

	m_tm = tm;
	m_scale = scale;
	m_pos = center;
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
		shader.BeginPass(i);
		shader.CommitChanges();

		m_vtxBuff.Bind(renderer);
		m_idxBuff.Bind(renderer);
		renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
			m_vtxBuff.GetVertexCount(), 0, 12);

		shader.EndPass();
	}
	shader.End();
}


void cCube3::RenderShader(cRenderer &renderer, const Matrix44 &tm)
{
	if (m_shader)
		RenderShader(renderer, *m_shader, tm);
}


void cCube3::SetColor(const DWORD color)
{
	sVertexNormDiffuseTex *vbuff = (sVertexNormDiffuseTex*)m_vtxBuff.Lock();
	for (int i = 0; i < m_vtxBuff.GetVertexCount(); ++i)
		vbuff[i].c = color;
	m_vtxBuff.Unlock();
}
