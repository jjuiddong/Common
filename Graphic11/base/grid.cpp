
#include "stdafx.h"
#include "grid.h"

using namespace graphic;


cGrid::cGrid()
	: cNode(common::GenerateId(), "grid", eNodeType::MODEL)
	, m_texture(NULL)
	, m_faceCount(0)
{
	m_mtrl.InitWhite();
}

cGrid::~cGrid()
{
}


// Grid 인덱스 버퍼를 생성할 때 쓰일, 배열을 생성한다.
template<class T>
void CreateIndexArray(const int rowCellCount, const int colCellCount
	, OUT vector<BYTE> &out)
{
	const int rowVtxCnt = rowCellCount + 1;
	const int colVtxCnt = colCellCount + 1;
	const int cellCnt = rowCellCount * colCellCount;

	const int addLineCount = rowCellCount + colCellCount;
	out.resize((cellCnt * 2 * 3) * sizeof(T), 0);
	T *indices = (T*)&out[0];
	{
		int baseIndex = 0;
		for (int i = 0; i < rowCellCount; ++i)
		{
			for (int k = 0; k < colCellCount; ++k)
			{
				//
				//  0 ----- 1,4
				//  |    /  |
				//  |   /   |
				//  |  /    |
				//  2,3 --- 5

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
	}
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
// uv(0,0)=uv0         uv(1,0)
// 0      1      2      3
// * ---- * ---- * ---- *
// |      |      |      |
// |4     | 5    |6     |7
// * ---- * - +  * ---- *
// |      |center|      |
// |8     | 9    | 10   |11
// * ---- * ---- * ---- *
// uv(0,1)             uv(1,1)=uv1
//
// Vertex Store Order
//
void cGrid::Create(cRenderer &renderer, const int rowCellCount, const int colCellCount
	, const float cellSizeW
	, const float cellSizeH
	, const int vertexType //= (eVertexType::POSITION | eVertexType::COLOR)
	, const cColor &color //= cColor::WHITE
	, const char *textureFileName //= g_defaultTexture
	, const Vector2 &uv0 //= Vector2(0, 0)
	, const Vector2 &uv1 //= Vector2(1, 1)
	, const float textureUVFactor // = 8.f
	, const bool isEditable //= false
)
{
	m_vtxType = vertexType;
	m_rowCellCount = rowCellCount;
	m_colCellCount = colCellCount;
	m_cellSizeW = cellSizeW;
	m_cellSizeH = cellSizeH;
	m_vtxType = vertexType;
	m_mtrl.m_diffuse = color.GetColor();

	cVertexLayout vtxLayout;
	vtxLayout.Create(vertexType);
	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();

	// Init Grid
	const int rowVtxCnt = rowCellCount + 1;
	const int colVtxCnt = colCellCount + 1;
	const int cellCnt = rowCellCount * colCellCount;
	const int vtxCount = rowVtxCnt * colVtxCnt;
	const Vector4 vcolor = color.GetColor();

	vector<BYTE> buffer0(vertexStride * vtxCount);
	BYTE *vertices = &buffer0[0];
	BYTE *pvtx = vertices;
	{
		const float startx = -cellSizeW*(colCellCount / 2);
		const float starty = cellSizeH*(rowCellCount / 2);
		const float endx = startx + cellSizeW*colCellCount;
		const float endy = starty - cellSizeH*rowCellCount;

		const float uCoordIncrementSize = (uv1.x - uv0.x) / (float)colCellCount * textureUVFactor;
		const float vCoordIncrementSize = (uv1.y - uv0.y) / (float)rowCellCount * textureUVFactor;

		int i = 0;
		for (float y = starty; y >= endy; y -= cellSizeH, ++i)
		{
			int k = 0;
			for (float x = startx; x <= endx; x += cellSizeW, ++k)
			{
				if (vertexType & eVertexType::POSITION)
					*(Vector3*)(pvtx + posOffset) = Vector3(x, 0, y);
				if (vertexType & eVertexType::NORMAL)
					*(Vector3*)(pvtx + normOffset) = Vector3(0, 1, 0);
				if (vertexType & eVertexType::COLOR)
					*(Vector4*)(pvtx + colorOffset) = vcolor;
				if (vertexType & eVertexType::TEXTURE0)
					*(Vector2*)(pvtx + texOffset) = uv0 + Vector2(k*uCoordIncrementSize, i*vCoordIncrementSize);
				pvtx += vertexStride;
			}
		}
	}

	m_vtxBuff.Create(renderer, vtxCount, vertexStride, &buffer0[0]
		, isEditable ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT);

	// Create Index Buffer
	const bool is32bit = WORD_MAX <= vtxCount;
	vector<BYTE> buffer1;
	if (is32bit)
		CreateIndexArray<DWORD>(rowCellCount, colCellCount, buffer1);
	else
		CreateIndexArray<WORD>(rowCellCount, colCellCount, buffer1);

	const int idxSizeOfBytes = is32bit ? sizeof(DWORD) : sizeof(WORD);
	const int faceCount = (buffer1.size() / idxSizeOfBytes) / 3;
	m_idxBuff.Create(renderer, faceCount, (BYTE*)&buffer1[0]
		, isEditable ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT
		, is32bit? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT);
	//

	m_faceCount = faceCount;
	m_texture = cResourceManager::Get()->LoadTexture(renderer, textureFileName);
	//m_texture = cResourceManager::Get()->LoadTextureParallel(renderer, textureFileName);
	//cResourceManager::Get()->AddParallelLoader(new cParallelLoader(cParallelLoader::eType::TEXTURE
	//	, textureFileName, (void**)&m_texture));

	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0)
		, Vector3(rowCellCount*cellSizeW*0.5f, 1, colCellCount*cellSizeH*0.5f)
		, Quaternion());
	CalcBoundingSphere();
}


bool cGrid::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);
	RETV(((m_renderFlags & flags) != flags), false);

	cShader11 *shader = (m_shader)? m_shader : renderer.m_shaderMgr.FindShader(m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM() * tm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbMaterial = m_mtrl.GetMaterial();
	renderer.m_cbMaterial.Update(renderer, 2);
	renderer.m_cbClipPlane.Update(renderer, 4);

	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);

	if ((m_vtxType & eVertexType::TEXTURE0) && m_texture)
		m_texture->Bind(renderer, 0);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_faceCount * 3, 0, 0);
	renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);

	__super::Render(renderer, tm, flags);
	return true;
}


bool cGrid::RenderLine(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);

	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	Transform tfm = m_transform;
	tfm.pos.y += 0.01f;
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm.GetMatrixXM() * tm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(0,0,0,0.6f);
	renderer.m_cbMaterial.Update(renderer, 2);
	renderer.m_cbClipPlane.Update(renderer, 4);

	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);

	if ((m_vtxType & eVertexType::TEXTURE0) && m_texture)
		m_texture->Bind(renderer, 0);

	CommonStates states(renderer.GetDevice());
	ID3D11RasterizerState *oldState = NULL;
	renderer.GetDevContext()->RSGetState(&oldState);
	renderer.GetDevContext()->RSSetState(states.Wireframe());
	renderer.GetDevContext()->OMSetBlendState(states.AlphaBlend(), 0, 0xffffffff);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_faceCount * 3, 0, 0);
	renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	renderer.GetDevContext()->RSSetState(oldState);

	return true;
}