
#include "stdafx.h"
#include "grid3.h"
#include <fstream>

using namespace graphic;


cGrid3::cGrid3() :
	m_rowCellCount(0)
	, m_colCellCount(0)
	, m_cellSize(0)
	, m_textureUVFactor(8)
	, m_tex(NULL)
{
	m_mtrl.InitWhite();
}

cGrid3::~cGrid3()
{
}


void cGrid3::Create(cRenderer &renderer
	, const int rowCellCount
	, const int colCellCount
	, const float cellSize
	, const float textureUVFactor // = 8.f
	, const float offsetY //= 0
	, const Vector2 &uv0 //= Vector2(0, 0)
	, const Vector2 &uv1 //= Vector2(1, 1)
)
{
	// init member
	m_rowCellCount = rowCellCount;
	m_colCellCount = colCellCount;
	m_cellSize = cellSize;
	m_textureUVFactor = textureUVFactor;

	// Init Grid
	const int rowVtxCnt = rowCellCount + 1;
	const int colVtxCnt = colCellCount + 1;
	const int cellCnt = rowCellCount * colCellCount;
	const int vtxCount = rowVtxCnt * colVtxCnt;

	m_vtxBuff.Create(renderer, vtxCount, sizeof(sVertexNormTex), sVertexNormTex::FVF);
	{
		sVertexNormTex *vertices = (sVertexNormTex*)m_vtxBuff.Lock();
		const float startx = -cellSize*(colCellCount / 2);
		const float starty = cellSize*(rowCellCount / 2);
		const float endx = startx + cellSize*colCellCount;
		const float endy = starty - cellSize*rowCellCount;

		//const float uCoordIncrementSize = 1.0f / (float)colCellCount * textureUVFactor;
		//const float vCoordIncrementSize = 1.0f / (float)rowCellCount * textureUVFactor;
		const float uCoordIncrementSize = (uv1.x - uv0.x) / (float)colCellCount * textureUVFactor;
		const float vCoordIncrementSize = (uv1.y - uv0.y) / (float)rowCellCount * textureUVFactor;

		int i = 0;
		for (float y = starty; y >= endy; y -= cellSize, ++i)
		{
			int k = 0;
			for (float x = startx; x <= endx; x += cellSize, ++k)
			{
				int index = (i * colVtxCnt) + k;
				vertices[index].p = Vector3(x, offsetY, y);
				vertices[index].n = Vector3(0, 1, 0);
				vertices[index].u = uv0.x + (float)k*uCoordIncrementSize;
				vertices[index].v = uv0.y + (float)i*vCoordIncrementSize;
			}
		}
		m_vtxBuff.Unlock();
	}


	m_idxBuff.Create(renderer, cellCnt * 2);
	{
		WORD *indices = (WORD*)m_idxBuff.Lock();
		int baseIndex = 0;
		for (int i = 0; i < rowCellCount; ++i)
		{
			for (int k = 0; k < colCellCount; ++k)
			{
				indices[baseIndex] = (i * colVtxCnt) + k;
				indices[baseIndex + 1] = (i   * colVtxCnt) + k + 1;
				indices[baseIndex + 2] = ((i + 1) * colVtxCnt) + k;

				indices[baseIndex + 3] = ((i + 1) * colVtxCnt) + k;
				indices[baseIndex + 4] = (i   * colVtxCnt) + k + 1;
				indices[baseIndex + 5] = ((i + 1) * colVtxCnt) + k + 1;

				// next quad
				baseIndex += 6;
			}
		}
		m_idxBuff.Unlock();
	}

}


// uv 값을 textureUVFactor 에 맞춰 재 조정한다.
void cGrid3::SetTextureUVFactor(const float textureUVFactor)
{
	// init member
	m_textureUVFactor = textureUVFactor;

	// 아직 grid가 초기화 되지 않았다면 uv 값을 조정하지 않는다.
	RET(m_rowCellCount <= 0);

	// Init Grid
	const int colVtxCnt = m_colCellCount + 1;;

	sVertexNormTex *vertices = (sVertexNormTex*)m_vtxBuff.Lock();
	const float startx = -m_cellSize*(m_colCellCount / 2);
	const float starty = m_cellSize*(m_rowCellCount / 2);
	const float endx = startx + m_cellSize*m_colCellCount;
	const float endy = starty - m_cellSize*m_rowCellCount;

	const float uCoordIncrementSize = 1.0f / (float)m_colCellCount * textureUVFactor;
	const float vCoordIncrementSize = 1.0f / (float)m_rowCellCount * textureUVFactor;

	int i = 0;
	for (float y = starty; y >= endy; y -= m_cellSize, ++i)
	{
		int k = 0;
		for (float x = startx; x <= endx; x += m_cellSize, ++k)
		{
			int index = (i * colVtxCnt) + k;
			vertices[index].u = (float)k*uCoordIncrementSize;
			vertices[index].v = (float)i*vCoordIncrementSize;
		}
	}
	m_vtxBuff.Unlock();
}


void cGrid3::Render(cRenderer &renderer, const Matrix44 &tm, const int stage)
{
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	renderer.GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	renderer.GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	renderer.GetDevice()->SetTransform(D3DTS_WORLD, ToDxM(tm));

	m_mtrl.Bind(renderer);
	if (m_tex)
		m_tex->Bind(renderer, stage);
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vtxBuff.GetVertexCount(),
		0, m_idxBuff.GetFaceCount());

	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}


void cGrid3::RenderLinelist(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, m_vtxBuff.GetVertexCount(),
		0, m_idxBuff.GetFaceCount() * 3 / 2);
}


void cGrid3::RenderShader(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(!m_shader);

	m_shader->SetMatrix("g_mWorld", tm);

	m_mtrl.Bind(*m_shader);
	if (m_tex)
		m_tex->Bind(*m_shader, "g_colorMapTexture");
	else
		m_shader->SetTexture("g_colorMapTexture", NULL);

	const int passCnt = m_shader->Begin();
	for (int i = 0; i < passCnt; ++i)
	{
		m_shader->BeginPass(i);
		m_shader->CommitChanges();
		m_vtxBuff.Bind(renderer);
		m_idxBuff.Bind(renderer);
		renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vtxBuff.GetVertexCount(),
			0, m_idxBuff.GetFaceCount());
		m_shader->EndPass();
	}
	m_shader->End();
}


// 법선 벡터를 다시 계산한다.
void cGrid3::CalculateNormals()
{
	sVertexNormTex *vertices = (sVertexNormTex*)m_vtxBuff.Lock();
	WORD *indices = (WORD*)m_idxBuff.Lock();
	for (int i = 0; i < m_idxBuff.GetFaceCount() * 3; i += 3)
	{
		sVertexNormTex &p1 = vertices[indices[i]];
		sVertexNormTex &p2 = vertices[indices[i + 1]];
		sVertexNormTex &p3 = vertices[indices[i + 2]];

		Vector3 v1 = p2.p - p1.p;
		Vector3 v2 = p3.p - p1.p;
		v1.Normalize();
		v2.Normalize();
		Vector3 n = v1.CrossProduct(v2);
		n.Normalize();
		p1.n = n;
		p2.n = n;
		p3.n = n;
	}

	m_vtxBuff.Unlock();
	m_idxBuff.Unlock();
}


// 광선 orig, dir 에 충돌된 면이 있다면 true 를 리턴하고, 충돌 위치를 out에 
// 저장해서 리턴한다.
bool cGrid3::Pick(const Vector3 &orig, const Vector3 &dir, Vector3 &out)
{
	bool isFirst = true;
	sVertexNormTex *vertices = (sVertexNormTex*)m_vtxBuff.Lock();
	WORD *indices = (WORD*)m_idxBuff.Lock();
	RETV(!vertices || !indices, false);

	for (int i = 0; i < m_idxBuff.GetFaceCount() * 3; i += 3)
	{
		const Vector3 &p1 = vertices[indices[i]].p;
		const Vector3 &p2 = vertices[indices[i + 1]].p;
		const Vector3 &p3 = vertices[indices[i + 2]].p;

		const Triangle tri(p1, p2, p3);
		const Plane p(p1, p2, p3);

		const float dot = dir.DotProduct(p.N);
		if (dot >= 0)
			continue;

		float t;
		if (tri.Intersect(orig, dir, &t))
		{
			if (isFirst)
			{
				isFirst = false;
				out = orig + dir * t;
			}
			else
			{
				const Vector3 v = orig + dir * t;
				if (orig.LengthRoughly(v) < orig.LengthRoughly(out))
					out = v;
			}
		}
	}
	m_vtxBuff.Unlock();
	m_idxBuff.Unlock();

	return !isFirst;
}


// 초기화.
void cGrid3::Clear()
{
	m_vtxBuff.Clear();
	m_idxBuff.Clear();
	m_tex = NULL;

	m_rowCellCount = 0;
	m_colCellCount = 0;
	m_cellSize = 0;
	m_textureUVFactor = 8;
}
