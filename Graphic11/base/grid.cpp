
#include "stdafx.h"
#include "grid.h"

using namespace graphic;


cGrid::cGrid() 
	: cNode2(common::GenerateId(), "grid", eNodeType::MODEL)
	, m_primitiveType(D3D11_PRIMITIVE_TOPOLOGY_LINELIST) // D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	, m_texture(NULL)
{
	m_mtrl.InitWhite();
}

cGrid::~cGrid()
{
}


void cGrid::Create(cRenderer &renderer, const int rowCellCount, const int colCellCount, const float cellSize
	, const int vertexType //= (eVertexType::POSITION | eVertexType::DIFFUSE)
	, const cColor &color //= cColor::WHITE
	, const char *textureFileName //= g_defaultTexture
	, const Vector2 &uv0 //= Vector2(0, 0)
	, const Vector2 &uv1 //= Vector2(1, 1)
	, const float textureUVFactor // = 8.f
)
{
	m_rowCellCount = rowCellCount;
	m_colCellCount = colCellCount;
	m_cellSize = cellSize;
	m_vertexType = vertexType;

	vector<D3D11_INPUT_ELEMENT_DESC> elems;
	if (vertexType & eVertexType::POSITION)
		elems.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vertexType & eVertexType::NORMAL)
		elems.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vertexType & eVertexType::DIFFUSE)
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
				if (vertexType & eVertexType::DIFFUSE)
					*(Vector4*)(pvtx + colorOffset) = vcolor;
				if (vertexType & eVertexType::TEXTURE)
					*(Vector2*)(pvtx + texOffset) = uv0 + Vector2(k*uCoordIncrementSize, i*vCoordIncrementSize);
				pvtx += vertexStride;
			}
		}
	}

	m_vtxBuff.Create(renderer, vtxCount, vertexStride, &buffer0[0]);

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

	m_idxBuff.Create(renderer, cellCnt * 2, &buffer1[0]);

	m_texture = cResourceManager::Get()->LoadTextureParallel(renderer, textureFileName);
	cResourceManager::Get()->AddParallelLoader(new cParallelLoader(cParallelLoader::eType::TEXTURE
		, textureFileName, (void**)&m_texture));

	if (vertexType & eVertexType::TEXTURE)
		m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}


bool cGrid::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
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

	__super::Render(renderer, tm, flags);
	return true;
}

