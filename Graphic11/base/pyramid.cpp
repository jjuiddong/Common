
#include "stdafx.h"
#include "pyramid.h"


using namespace graphic;


cPyramid::cPyramid()
{
}

cPyramid::~cPyramid()
{
}


bool cPyramid::Create(cRenderer &renderer
	, const float width //=1
	, const float height//=1
	, const Vector3 &pos //=Vector3(0,0,0)
	, const DWORD color //=0
)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return true; // alread created

//
//            0
//           /  \
//          /    \
//       1,2 ----- 3,4
//
//       1 ----------- 3
//       | \           |
//       |    \        |
//       |      +      |
//       |       \     |
//       |          \  |
//       2 ----------- 4
//
//				Z
//				|
//				|
//				|
//        ----------------> X
//

	Vector3 vertices[] = {
		Vector3(0,1,0)
		, Vector3(-0.5f,0,0.5f)
		, Vector3(-0.5f,0,-0.5f)
		, Vector3(0.5f,0,0.5f)
		, Vector3(0.5f,0,-0.5f)
	};

	int indices[] = {
		// bottom
		1,2,4
		,1,4,3
		,0,1,3 // Z axis
		,0,3,4 // X axis
		,0,4,2 // -Z axis
		,0,2,1 // -X axis
	};

	if (!m_vtxBuff.Create(renderer, ARRAYSIZE(vertices), sizeof(sVertexDiffuse), sVertexDiffuse::FVF))
		return false;

	if (!m_idxBuff.Create(renderer, ARRAYSIZE(indices)/3))
		return false;

	if (sVertexDiffuse *p = (sVertexDiffuse*)m_vtxBuff.Lock())
	{
		for (auto &v : vertices)
		{
			p->p = v;
			p->c = color;
			++p;
		}
		m_vtxBuff.Unlock();
	}

	if (WORD *idx = (WORD*)m_idxBuff.Lock())
	{
		for (int i = 0; i < ARRAYSIZE(indices); ++i)
			*idx++ = indices[i];
		m_idxBuff.Unlock();
	}


	m_transform.scale = Vector3(width, height, width);
	m_transform.pos = pos;
	return true;
}


void cPyramid::Render(cRenderer &renderer
	, const Matrix44 &tm //=Matrix44::Identity
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

	Matrix44 mat = m_transform.GetMatrix() * tm;
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&mat);
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		m_vtxBuff.GetVertexCount(), 0, 12);

	renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, cullMode);
	renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, fillMode);
	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lightMode);
}


void cPyramid::SetDimension(const float width, const float height)
{
	m_transform.scale = Vector3(width, height, width);
}


void cPyramid::SetPos(const Vector3 &pos)
{
	m_transform.pos = pos;
}


// Rotation Head to (p1-p0) Vector
void cPyramid::SetDirection(const Vector3 &p0, const Vector3 &p1
	, const float width //= 1
)
{
	Vector3 v = p1 - p0;
	Quaternion q;
	q.SetRotationArc(Vector3(0, 1, 0), v.Normal());
	m_transform.rot = q;
	m_transform.pos = p0;
	m_transform.scale = Vector3(width, v.Length(), width);
}


void cPyramid::SetDirection(const Vector3 &p0, const Vector3 &p1, const Vector3 &from
	, const float width //= 1
)
{
	Vector3 v = p1 - p0;
	Quaternion q;
	q.SetRotationArc(Vector3(0, 1, 0), v.Normal());
	m_transform.rot = q;
	m_transform.pos = from;
	m_transform.scale = Vector3(width, v.Length(), width);
}
