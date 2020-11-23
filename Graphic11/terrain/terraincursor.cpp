
#include "StdAfx.h"
#include "TerrainCursor.h"


using namespace graphic;

cTerrainCursor::cTerrainCursor() 
	: cNode(common::GenerateId(), "Terrain Cursor", eNodeType::MODEL)
	, m_innerRadius(10)
	, m_outerRadius(20)
	, m_innerAlpha(1.f)
	, m_brushSpeed(60.f)
{
}


cTerrainCursor::~cTerrainCursor()
{
}


bool cTerrainCursor::Create(cRenderer &renderer)
{
	m_innerCircle.Create(renderer, CURSOR_VERTEX_COUNT, sizeof(sVertexDiffuse), D3D11_USAGE_DYNAMIC);
	m_outerCircle.Create(renderer, CURSOR_VERTEX_COUNT, sizeof(sVertexDiffuse), D3D11_USAGE_DYNAMIC);
	return true;
}


bool cTerrainCursor::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(eVertexType::POSITION | eVertexType::COLOR);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.UnbindTextureAll();
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	DirectX::CommonStates common(renderer.GetDevice());
	renderer.GetDevContext()->OMSetDepthStencilState(common.DepthNone(), 0xffffffff);
	{
		m_innerCircle.Bind(renderer);
		renderer.GetDevContext()->DrawInstanced(CURSOR_VERTEX_COUNT, 1, 0, 0);
		m_outerCircle.Bind(renderer);
		renderer.GetDevContext()->DrawInstanced(CURSOR_VERTEX_COUNT, 1, 0, 0);
	}
	renderer.GetDevContext()->OMSetDepthStencilState(common.DepthDefault(), 0xffffffff);

	return true;
}


void cTerrainCursor::UpdateCursor(cRenderer &renderer, cTerrain &terrain, const Vector3 &cursorPos )
{
	m_pos = cursorPos;

	sVertexDiffuse *innerVertices = (sVertexDiffuse*)m_innerCircle.Lock(renderer);
	sVertexDiffuse *outerVertices = (sVertexDiffuse*)m_outerCircle.Lock(renderer);
	if (!innerVertices || !outerVertices)
	{
		m_innerCircle.Unlock(renderer);
		m_outerCircle.Unlock(renderer);
		return;
	}

	float angle = 0.f;
	int index = 0;
	const float offset = MATH_PI*2.f / (float)(CURSOR_VERTEX_COUNT-1);

	while ((angle < MATH_PI*2.f) && (index < CURSOR_VERTEX_COUNT))
	{
		Vector3 inner(m_innerRadius*cos(angle), 0.f, m_innerRadius*sin(angle));
		inner += cursorPos;
		innerVertices[ index].p = inner;
		innerVertices[ index].p.y = terrain.GetHeight(inner.x, inner.z);
		innerVertices[index].c = Vector4(0, 0, 0, 1);

		Vector3 outer(m_outerRadius*cos(angle), 0.f, m_outerRadius*sin(angle));
		outer += cursorPos;
		outerVertices[ index].p = outer;
		outerVertices[ index].p.y = terrain.GetHeight(outer.x, outer.z);
		outerVertices[index].c = Vector4(0, 0, 0, 1);

		index++;
		angle += offset;
	}

	m_innerCircle.Unlock(renderer);
	m_outerCircle.Unlock(renderer);
}


// Terrain Geometry Up
// cursorPos : 카메라에서 프로젝션된 후, 선택된 맵의 x,z 위치를 가르킨다.
void cTerrainCursor::GeometryEdit(cRenderer &renderer
	, cTerrain &terrain
	, const eGeoEditType::Enum editType
	, const Vector3 &cursorPos
	, const float deltaSeconds
	, const bool isHilightTile //= false
)
{
	for (auto p : terrain.m_tiles)
		p->m_isHilight = false;

	// Check Intersect Tile in Cursor Region
	vector<cTile*> tiles;
	cBoundingSphere bsphere(cursorPos, m_outerRadius);
	for (auto p : terrain.m_tiles)
	{
		cBoundingSphere tileSphere = p->m_boundingSphere;
		tileSphere *= p->GetWorldMatrix();
		if (tileSphere.Intersects(bsphere))
			tiles.push_back(p);
	}

	if (tiles.empty())
		return;

	for (auto p : tiles)
		p->m_isHilight = isHilightTile;

	float brushSpeed = m_brushSpeed;
	switch (editType)
	{
	case eGeoEditType::UP: break;
	case eGeoEditType::FLAT: break;
	case eGeoEditType::DOWN: brushSpeed = -brushSpeed;  break;
	default: assert(0); break;
	}

	// Update Heightmap
	for (uint i=0; i < terrain.m_heightMap.size(); ++i)
	{
		auto &vtx = terrain.m_heightMap[i];
		const Vector2 p0(vtx.p.x, vtx.p.z);
		const Vector2 p1(cursorPos.x, cursorPos.z);
		const float len = (p0 - p1).Length();
		if (len <= m_outerRadius)
		{
			switch (editType)
			{
			case eGeoEditType::UP:
			case eGeoEditType::DOWN:
			{
				const float offsetY = brushSpeed * deltaSeconds;
				vtx.p = vtx.p + (Vector3(0, 1, 0) * offsetY * (m_outerRadius - len) / m_outerRadius);
			}
			break;

			case eGeoEditType::FLAT:
			{
				// average left,right,up,down height
				const int row = i / terrain.m_colVtx;
				const int col = i % terrain.m_colVtx;

				int left;
				{
					const int r = row;
					const int c = max(0, col - 1);
					left = r*terrain.m_colVtx + c;
				}
				int right;
				{
					const int r = row;
					const int c = min(terrain.m_colVtx-1, col + 1);
					right = r*terrain.m_colVtx + c;
				}
				int top;
				{
					const int r = min(terrain.m_rowVtx-1, row + 1);
					const int c = col;
					top = r*terrain.m_colVtx + c;
				}
				int bottom;
				{
					const int r = max(0, row - 1);
					const int c = col;
					bottom = r*terrain.m_colVtx + c;
				}

				float h = 0;
				h += terrain.m_heightMap[left].p.y;
				h += terrain.m_heightMap[right].p.y;
				h += terrain.m_heightMap[top].p.y;
				h += terrain.m_heightMap[bottom].p.y;
				h /= 4.f; // average
				
				const float curH = terrain.m_heightMap[i].p.y;
				const float offsetY = brushSpeed * deltaSeconds * 0.3f;
				vtx.p = vtx.p + (Vector3(0, h - curH, 0) * offsetY * (m_outerRadius - len) / m_outerRadius);
			}
			break;
			}
		}
	}

	terrain.HeightmapNormalize(cursorPos, m_outerRadius);
	terrain.UpdateHeightmapToTile(renderer, &tiles[0], tiles.size());
}
