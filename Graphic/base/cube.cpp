
#include "stdafx.h"
#include "cube.h"

using namespace graphic;


cCube::cCube()
	: m_scale(1)
{
}

cCube::cCube(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax)
{
	InitCube(renderer);
	SetCube(renderer, vMin, vMax);
}


void cCube::InitCube(cRenderer &renderer)
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
	m_idxBuff.Create(renderer, 12);

	if (sVertexNormDiffuse *vbuff = (sVertexNormDiffuse*)m_vtxBuff.Lock())
	{
		const DWORD color = D3DCOLOR_XRGB(200, 200, 200);
		for (int i = 0; i < 36; ++i)
		{
			vbuff[i].p = vertices[indices[i]];
			vbuff[i].n = normals[normalIndices[i]];
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


void cCube::SetCube(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax)
{
	if (m_vtxBuff.GetVertexCount() <= 0)
		InitCube(renderer);

	const Vector3 center = (vMin + vMax) / 2.f;
	const Vector3 v1 = vMin - vMax;
	const Vector3 v2 = m_max - m_min;
	Vector3 scale(abs(v1.x)/2, abs(v1.y)/2, abs(v1.z)/2);

	Matrix44 S;
	S.SetScale(scale);
	Matrix44 T;
	T.SetTranslate(center);
	Matrix44 tm = S * T;

	m_tm = tm;
	m_scale = scale;
	m_pos = center;
	m_min = vMin;
	m_max = vMax;
}


void cCube::SetCube(cRenderer &renderer, const cCube &cube)
{
	SetCube(renderer, cube.GetMin(), cube.GetMax());
	m_tm = cube.GetTransform();
}


void cCube::SetColor( DWORD color )
{
	sVertexNormDiffuse *vbuff = (sVertexNormDiffuse*)m_vtxBuff.Lock();
	for (int i=0; i < m_vtxBuff.GetVertexCount(); ++i)
		vbuff[ i].c = color;
	m_vtxBuff.Unlock();
}


void cCube::Render(cRenderer &renderer, const Matrix44 &tm)
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


void cCube::RenderSolid(cRenderer &renderer, const Matrix44 &tm)
{
	Matrix44 mat = m_tm * tm;
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&mat);
	renderer.GetDevice()->SetTexture(0, NULL);
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		m_vtxBuff.GetVertexCount(), 0, 12);
}


void cCube::RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &tm)
{
	shader.SetMatrix("g_mWorld", m_tm*tm);

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


void cCube::RenderShader(cRenderer &renderer, const Matrix44 &tm)
{
	if (m_shader)
		RenderShader(renderer, *m_shader, tm);
}


void cCube::ReCalcTransform()
{
	Matrix44 S;
	S.SetScale(m_scale);
	Matrix44 T;
	T.SetTranslate(m_pos);
	Matrix44 tm = S * T;
	m_tm = tm;
}
