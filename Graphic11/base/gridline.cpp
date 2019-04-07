
#include "stdafx.h"
#include "gridline.h"

using namespace graphic;


cGridLine::cGridLine()
	: cNode(common::GenerateId(), "gridline", eNodeType::MODEL)
	, m_lineColor(0.7f, 0.7f, 0.7f, 0.7f)
	//, m_offsetY(0.1f)
	//, m_offsetY(0.001f)
	, m_offsetY(0.0f)
{
}

cGridLine::~cGridLine()
{
}


// 
//   z
//   |
//   |
//   |
//   |
//  -|--------------> x
//  Axis X-Z
//
//
// 0,8    9      10      4,11
// * ---- * ---- * ---- *
// |      |      |      |
// |1     |      |      | 
// * ------------------ * 5
// |      |      |      |
// |2     |  +   |      | 
// * ------------------ * 6
// |      |      |      |
// |3,12  | 13   | 14   | 7,15
// * ---- * ---- * ---- *
//
// Vertex Store Order
//
void cGridLine::Create(cRenderer &renderer
	, const int rowCellCount
	, const int colCellCount
	, const float cellSizeW
	, const float cellSizeH
	, const int vertexType //= (eVertexType::POSITION | eVertexType::COLOR)
	, const cColor &color1 //= cColor(0.6f, 0.6f, 0.6f, 1.f)
	, const cColor &color2 //= cColor(1.f, 1.f, 1.f, 1.f)
	, const bool isCell5 //= true
	, const bool isAxis //= true
)
{
	// 가로,세로 셀갯수는 항상 짝수여야 한다. (일단 5의 배수만 가능하게 함)
	// 그래야 중심 라인 X,Z 축 라인을 그릴 수 있다.
	assert(((rowCellCount % 5) == 0) && ((colCellCount % 5) == 0));

	m_vtxType = vertexType;
	m_rowCellCount = rowCellCount;
	m_colCellCount = colCellCount;
	m_cellSizeW = cellSizeW;
	m_cellSizeH = cellSizeH;
	m_vtxType = vertexType;

	cVertexLayout vtxLayout;
	vtxLayout.Create(vertexType);
	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int vertexStride = vtxLayout.GetVertexSize();

	// Init Grid
	const int rowVtxCnt = rowCellCount + 1;
	const int colVtxCnt = colCellCount + 1;
	const int vtxCount = (rowVtxCnt * 2) + (colVtxCnt * 2);
	const Vector4 vcolor1 = color1.GetColor();
	const Vector4 vcolor2 = color2.GetColor();

	vector<BYTE> buffer0(vertexStride * vtxCount);
	BYTE *vertices = &buffer0[0];
	BYTE *pvtx = vertices;
	{
		const float startx = -cellSizeW*(colCellCount / 2);
		const float starty = cellSizeH*(rowCellCount / 2);
		const float endx = startx + cellSizeW*colCellCount;
		const float endy = starty - cellSizeH*rowCellCount;

		// Row Vertex (Start Point)
		for (int i = 0; i < rowVtxCnt; ++i)
		{
			if (vertexType & eVertexType::POSITION)
				*(Vector3*)(pvtx + posOffset) = Vector3(startx, 0, starty - i*cellSizeH);
			if (vertexType & eVertexType::COLOR)
			{
				if (isCell5 && ((i % 5) == 0))
				{
					if (isAxis && (i == (rowCellCount/2)))
						*(Vector4*)(pvtx + colorOffset) = Vector4(1,0,0,1); // X Axis Color
					else
						*(Vector4*)(pvtx + colorOffset) = vcolor2;
				}
				else
				{
					*(Vector4*)(pvtx + colorOffset) = vcolor1;
				}
			}
			pvtx += vertexStride;
		}

		// Row Vertex (End Point)
		for (int i = 0; i < rowVtxCnt; ++i)
		{
			if (vertexType & eVertexType::POSITION)
				*(Vector3*)(pvtx + posOffset) = Vector3(endx, 0, starty - i*cellSizeH);
			if (vertexType & eVertexType::COLOR)
			{
				if (isCell5 && ((i % 5) == 0))
				{
					if (isAxis && (i == (rowCellCount / 2)))
						*(Vector4*)(pvtx + colorOffset) = Vector4(1, 0, 0, 1); // X Axis Color
					else
						*(Vector4*)(pvtx + colorOffset) = vcolor2;
				}
				else
				{
					*(Vector4*)(pvtx + colorOffset) = vcolor1;
				}
			}
			pvtx += vertexStride;
		}

		// Col Vertex (Start Point)
		for (int i = 0; i < colVtxCnt; ++i)
		{
			if (vertexType & eVertexType::POSITION)
				*(Vector3*)(pvtx + posOffset) = Vector3(startx + i*cellSizeW, 0, starty);
			if (vertexType & eVertexType::COLOR)
			{
				if (isCell5 && ((i % 5) == 0))
				{
					if (isAxis && (i == (colCellCount / 2)))
						*(Vector4*)(pvtx + colorOffset) = Vector4(0, 0, 1, 1); // Z Axis Color
					else
						*(Vector4*)(pvtx + colorOffset) = vcolor2;
				}
				else
				{
					*(Vector4*)(pvtx + colorOffset) = vcolor1;
				}
			}
			pvtx += vertexStride;
		}

		// Col Vertex (End Point)
		for (int i = 0; i < colVtxCnt; ++i)
		{
			if (vertexType & eVertexType::POSITION)
				*(Vector3*)(pvtx + posOffset) = Vector3(startx + i*cellSizeW, 0, endy);
			if (vertexType & eVertexType::COLOR)
			{
				if (isCell5 && ((i % 5) == 0))
				{
					if (isAxis && (i == (colCellCount / 2)))
						*(Vector4*)(pvtx + colorOffset) = Vector4(0, 0, 1, 1); // Z Axis Color
					else
						*(Vector4*)(pvtx + colorOffset) = vcolor2;
				}
				else
				{
					*(Vector4*)(pvtx + colorOffset) = vcolor1;
				}
			}
			pvtx += vertexStride;
		}
	}

	m_vtxBuff.Create(renderer, vtxCount, vertexStride, &buffer0[0]);

	// Create Index Buffer
	vector<WORD> buffer1;
	buffer1.reserve(vtxCount * 2);
	for (int i = 0; i < rowVtxCnt; ++i)
	{
		buffer1.push_back(i);
		buffer1.push_back(i+rowVtxCnt);
	}
	for (int i = 0; i < colVtxCnt; ++i)
	{
		buffer1.push_back((rowVtxCnt * 2) + i);
		buffer1.push_back((rowVtxCnt * 2) + i + colVtxCnt);
	}
	//

	const int idxSizeOfBytes = sizeof(WORD);
	const int faceCount = buffer1.size() / 2;
	m_idxBuff.Create(renderer, faceCount, (BYTE*)&buffer1[0]
		, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R16_UINT, 2);

	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0)
		, Vector3(rowCellCount*cellSizeW*0.5f, 1, colCellCount*cellSizeH*0.5f)
		, Quaternion());
	CalcBoundingSphere();
}


bool cGridLine::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);
	RETV(((m_renderFlags & flags) != flags), false);

	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	Transform tfm = m_transform;
	tfm.pos.y += m_offsetY;
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm.GetMatrixXM() * tm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbMaterial.m_v->diffuse = XMLoadFloat4((XMFLOAT4*)&m_lineColor.GetColor());
	renderer.m_cbMaterial.Update(renderer, 2);
	renderer.m_cbClipPlane.Update(renderer, 4);

	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 2, 0, 0);
	renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);

	// recovery material
	renderer.m_cbMaterial.m_v->diffuse = XMLoadFloat4((XMFLOAT4*)&Vector4(1, 1, 1, 1));

	// debugging
	++renderer.m_drawCallCount;
#ifdef _DEBUG
	++renderer.m_shadersDrawCall[m_vtxType];
#endif

	return __super::Render(renderer, tm, flags);
}
