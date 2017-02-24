
#include "stdafx.h"
#include "line3.h"

using namespace graphic;

cLine3::cLine3()
{
	m_mtrl.InitWhite();
}

cLine3::cLine3(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width)
{
	SetLine(renderer, p0, p1, width);
}


void cLine3::Render(cRenderer &renderer, const Matrix44 &tm)
//tm = Matrix44::Identity
{
	DWORD lighting;
	renderer.GetDevice()->GetRenderState(D3DRS_LIGHTING, &lighting);
	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

	Matrix44 m = m_tm * tm;

	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&m);
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		m_vtxBuff.GetVertexCount(), 0, 12);

	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lighting);
}


void cLine3::RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &tm)
//tm = Matrix44::Identity
{
	//const cLight &mainLight = cLightManager::Get()->GetMainLight();
	//mainLight.Bind(shader);
	//shader.SetVector("g_vEyePos", cMainCamera::Get()->GetEyePos());

	shader.SetMatrix("g_mWorld", m_tm*tm);
	//shader.SetMatrix("g_mVP", GetMainCamera()->GetViewProjectionMatrix());

	m_mtrl.Bind(shader);
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	shader.CommitChanges();

	const int passCount = shader.Begin();
	for (int i = 0; i < passCount; ++i)
	{
		shader.BeginPass(0);
		renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
			m_vtxBuff.GetVertexCount(), 0, 12);
		shader.EndPass();
	}
	shader.End();

}


void cLine3::SetLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width
	, const D3DCOLOR color) //color=0
{
	m_p0 = p0;
	m_p1 = p1;
	m_width = width;

	InitCube(renderer, color);

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


void cLine3::InitCube(cRenderer &renderer, const D3DCOLOR color)
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

	m_mtrl.InitWhite();
}
