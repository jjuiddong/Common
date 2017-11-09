
#include "stdafx.h"
#include "grid.h"

using namespace graphic;


cGrid::cGrid() 
	: cNode(common::GenerateId(), "grid", eNodeType::MODEL)
	, m_primitiveType(D3D11_PRIMITIVE_TOPOLOGY_LINELIST) // D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	, m_texture(NULL)
	, m_isLineDrawing(false)
{
	m_mtrl.InitWhite();
}

cGrid::~cGrid()
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
void cGrid::Create(cRenderer &renderer, const int rowCellCount, const int colCellCount, const float cellSize
	, const int vertexType //= (eVertexType::POSITION | eVertexType::COLOR)
	, const cColor &color //= cColor::WHITE
	, const char *textureFileName //= g_defaultTexture
	, const Vector2 &uv0 //= Vector2(0, 0)
	, const Vector2 &uv1 //= Vector2(1, 1)
	, const float textureUVFactor // = 8.f
	, const bool isEditable //= false
)
{
	m_rowCellCount = rowCellCount;
	m_colCellCount = colCellCount;
	m_cellSize = cellSize;
	m_vertexType = vertexType;
	m_mtrl.m_diffuse = color.GetColor();

	vector<D3D11_INPUT_ELEMENT_DESC> elems;
	if (vertexType & eVertexType::POSITION)
		elems.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vertexType & eVertexType::NORMAL)
		elems.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vertexType & eVertexType::COLOR)
		elems.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vertexType & eVertexType::TEXTURE)
		elems.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	cVertexLayout vtxLayout;
	vtxLayout.Create(elems);

	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();

	// Init Grid
	const int rowVtxCnt  = rowCellCount+1;
	const int colVtxCnt  = colCellCount+1;
	const int cellCnt = rowCellCount * colCellCount;
	const int vtxCount = rowVtxCnt * colVtxCnt;
	const Vector4 vcolor = color.GetColor();

	vector<BYTE> buffer0(vertexStride * vtxCount);
	BYTE *vertices = &buffer0[0];
	BYTE *pvtx = vertices;
	{
		const float startx = -cellSize*(colCellCount / 2);
		const float starty = cellSize*(rowCellCount / 2);
		const float endx = startx + cellSize*colCellCount;
		const float endy = starty - cellSize*rowCellCount;

		const float uCoordIncrementSize = (uv1.x - uv0.x) / (float)colCellCount * textureUVFactor;
		const float vCoordIncrementSize = (uv1.y - uv0.y) / (float)rowCellCount * textureUVFactor;

		int i = 0;
		for (float y = starty; y >= endy; y -= cellSize, ++i)
		{
			int k = 0;
			for (float x = startx; x <= endx; x += cellSize, ++k)
			{
				if (vertexType & eVertexType::POSITION)
					*(Vector3*)(pvtx + posOffset) = Vector3(x, 0, y);
				if (vertexType & eVertexType::NORMAL)
					*(Vector3*)(pvtx + normOffset) = Vector3(0, 1, 0);
				if (vertexType & eVertexType::COLOR)
					*(Vector4*)(pvtx + colorOffset) = vcolor;
				if (vertexType & eVertexType::TEXTURE)
					*(Vector2*)(pvtx + texOffset) = uv0 + Vector2(k*uCoordIncrementSize, i*vCoordIncrementSize);
				pvtx += vertexStride;
			}
		}
	}

	m_vtxBuff.Create(renderer, vtxCount, vertexStride, &buffer0[0]
		, isEditable? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT);

	vector<WORD> buffer1(cellCnt * 2 * 3);
	WORD *indices = &buffer1[0];
	{
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
	}

	m_idxBuff.Create(renderer, cellCnt * 2, (BYTE*)&buffer1[0]
		, isEditable ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT);

	m_texture = cResourceManager::Get()->LoadTextureParallel(renderer, textureFileName);
	cResourceManager::Get()->AddParallelLoader(new cParallelLoader(cParallelLoader::eType::TEXTURE
		, textureFileName, (void**)&m_texture));

	if (vertexType & eVertexType::TEXTURE)
		m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0)
		, Vector3(rowCellCount*cellSize*0.5f, 1, colCellCount*cellSize*0.5f)
		, Quaternion());
	CalcBoundingSphere();
}


bool cGrid::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	if (!(flags & m_renderFlags))
		return false;

	cShader11 *shader = (m_shader)? m_shader : renderer.m_shaderMgr.FindShader(m_vertexType);
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

	if ((m_vertexType & eVertexType::TEXTURE) && m_texture)
		m_texture->Bind(renderer, 0);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
	//renderer.GetDevContext()->OMSetBlendState(states.AlphaBlend(), 0, 0xffffffff);
	renderer.GetDevContext()->IASetPrimitiveTopology(m_primitiveType);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 3, 0, 0);
	renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);

	// Line Drawing (ignore shadow map building)
	if (m_isLineDrawing && !(flags & eRenderFlag::SHADOW))
	{
		shader->SetTechnique(m_techniqueName.c_str());
		shader->Begin();
		shader->BeginPass(renderer, 0);

		Transform tfm = m_transform;
		tfm.pos.y += 0.1f;
		renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm.GetMatrixXM() * tm);
		renderer.m_cbPerFrame.Update(renderer);
		renderer.m_cbLight.Update(renderer, 1);
		renderer.m_cbMaterial.m_v->diffuse = XMLoadFloat4((XMFLOAT4*)&Vector4(0.7f, 0.7f, 0.7f, 0.5f));
		renderer.m_cbMaterial.Update(renderer, 2);
		renderer.m_cbClipPlane.Update(renderer, 4);

		m_vtxBuff.Bind(renderer);
		m_idxBuff.Bind(renderer);

		if ((m_vertexType & eVertexType::TEXTURE) && m_texture)
			m_texture->Bind(renderer, 0);

		renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
		renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		renderer.m_cbMaterial.m_v;
		renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 3, 0, 0);
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);

		// recovery material
		renderer.m_cbMaterial.m_v->diffuse = XMLoadFloat4((XMFLOAT4*)&Vector4(1, 1, 1, 1));
	}

	__super::Render(renderer, tm, flags);
	return true;
}

