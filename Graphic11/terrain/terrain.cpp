
#include "stdafx.h"
#include "terrain.h"
#include "tile.h"


using namespace graphic;


cTerrain::cTerrain()
	: cNode(common::GenerateId(), "terrain", eNodeType::TERRAIN)
	, m_isShowDebug(false)
	, m_rows(0)
	, m_cols(0)
	, m_tileCols(0)
	, m_tileRows(0)
	, m_defaultHeight(0)
	, m_lerpAlphaFactor(0.02f)
{
	SetRenderFlag(eRenderFlag::TERRAIN, true);
}

cTerrain::~cTerrain()
{
	Clear();
}


bool cTerrain::Create(cRenderer &renderer, const sRectf &rect)
{
	return true;
}


bool cTerrain::Create(cRenderer &renderer, const int rowCnt, const int colCnt
	, const float cellSizeW
	, const float cellSizeH
	, const int rowTileCnt, const int colTileCnt)
{
	m_rows = rowCnt;
	m_cols = colCnt;
	m_cellSize = Vector2(cellSizeW, cellSizeH);
	m_rowVtx = rowCnt + 1;
	m_colVtx = colCnt + 1;
	m_tileCols = colTileCnt;
	m_tileRows = rowTileCnt;
	m_rect = sRectf(0, 0, colCnt*cellSizeW, rowCnt*cellSizeH);
	const float tileWidth = colCnt*cellSizeW / colTileCnt;
	const float tileHeight = rowCnt*cellSizeH / rowTileCnt;
	m_heightMap.resize(m_rowVtx * m_colVtx);

	// initialize map
	// 
	//   z (row)
	//   |
	//   |
	//   |
	//   |
	//  -|--------------> x (column)
	//  Axis X-Z
	//
	//
	// 8      9      10      11
	// * ---- * ---- * ---- *
	// |      |      |      |
	// |4     | 5    |6     |7
	// * ---- * ---- * ---- *
	// |      |      |      |
	// |0     | 1    | 2    |3
	// * ---- * ---- * ---- *
	//
	// Vertex Store Order
	//
	for (int r = 0; r < rowCnt+1; ++r)
	{
		for (int c = 0; c < colCnt+1; ++c)
		{
			Vector3 p(c*cellSizeW - (tileWidth/2.f), 0, r*cellSizeH - (tileHeight/2.f));
			Vector3 n(0, 1, 0);
			const int idx = r*(colCnt+1) + c;
			m_heightMap[idx].p = p;
			m_heightMap[idx].n = n;
		}
	}

	return true;
}


void cTerrain::BuildCascadedShadowMap(cRenderer &renderer
	, INOUT cCascadedShadowMap &ccsm
	, const XMMATRIX &tm //= XMIdentity
)
{
	ccsm.UpdateParameter(renderer, GetMainCamera());
	
	SetTechnique("BuildShadowMap");

	for (int i = 0; i < cCascadedShadowMap::SHADOWMAP_COUNT; ++i)
	{
		for (auto &p : m_tiles)
			p->CullingTest(ccsm.m_frustums[i], tm, true);

		ccsm.Begin(renderer, i);
		for (auto &p : m_tiles)
			p->PreRender(renderer, tm, eRenderFlag::SHADOW);
		ccsm.End(renderer, i);
	}

	CullingTestOnly(renderer, GetMainCamera()); // Recovery Culling
}


bool cTerrain::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	GetMainCamera().Bind(renderer);
	GetMainLight().Bind(renderer);

	CullingTestOnly(renderer, GetMainCamera(), true, tm); // Recovery Culling
	__super::Render(renderer, tm, eRenderFlag::TERRAIN);
	__super::Render(renderer, tm, eRenderFlag::MODEL);
	return true;
}


bool cTerrain::RenderCascadedShadowMap(cRenderer &renderer
	, cCascadedShadowMap &ccsm
	, const XMMATRIX &tm //= XMIdentity
	, const int flags // =1
)
{
	GetMainCamera().Bind(renderer);
	GetMainLight().Bind(renderer);

	ccsm.Bind(renderer);
	__super::Render(renderer, tm, eRenderFlag::TERRAIN);
	__super::Render(renderer, tm, eRenderFlag::MODEL | eRenderFlag::NOALPHABLEND);
	
	if (m_isShowDebug)
		ccsm.DebugRender(renderer);

	return true;
}


void cTerrain::RenderOption(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int option //= 0x1
)
{
	GetMainCamera().Bind(renderer);
	GetMainLight().Bind(renderer);

	__super::Render(renderer, tm, option);
}


void cTerrain::RenderDebug(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	GetMainCamera().Bind(renderer);
	GetMainLight().Bind(renderer);
}


void cTerrain::CullingTestOnly(cRenderer &renderer, cCamera &camera
	, const bool isModel //= true
	, const XMMATRIX &tm //= XMIdentity
)
{
	cFrustum frustum;
	frustum.SetFrustum(camera.GetViewProjectionMatrix());

	for (auto &p : m_tiles)
		p->CullingTest(frustum, tm, isModel);
}


bool cTerrain::AddTile(cTile *tile)
{
	AddChild(tile);

	auto it = std::find(m_tiles.begin(), m_tiles.end(), tile);
	if (m_tiles.end() != it)
		return false; // Already Exist

	m_tiles.push_back(tile);
	m_tilemap[tile->m_name.GetHashCode()] = tile;
	m_tilemap2[tile->m_id] = tile;
	return true;
}


bool cTerrain::RemoveTile(cTile *tile
	, const bool rmInstance //= true
)
{
	RemoveChild(tile, rmInstance);

	auto it = std::find(m_tiles.begin(), m_tiles.end(), tile);
	if (m_tiles.end() == it)
		return false; // Not Exist

	common::popvector2(m_tiles, tile);
	m_tilemap.erase(tile->m_name.GetHashCode());
	m_tilemap2.erase(tile->m_id);
	return true;
}


// insert model to most nearest tile
bool cTerrain::AddModel(cNode *model)
{
	RETV(m_tiles.empty(), false);

	cTile *nearTile = GetNearestTile(model);
	if (!nearTile)
		nearTile = m_tiles[0];

	model->m_transform = model->m_transform * nearTile->m_transform.Inverse();
	model->CalcBoundingSphere();

	return nearTile->AddChild(model);
}


// Update Model Position
bool cTerrain::UpdateModel(cNode *model)
{
	cTile *nearTile = GetNearestTile(model);
	RETV(!nearTile, false);

	Transform transform = model->GetWorldTransform();
	model->m_transform = transform * nearTile->m_transform.Inverse();
	model->CalcBoundingSphere();

	if (model->m_parent)
		model->m_parent->RemoveChild(model, false);

	return nearTile->AddChild(model);
}


cTile* cTerrain::GetNearestTile(const cNode *node)
{
	cBoundingBox bbox = node->m_boundingBox;
	bbox *= node->GetWorldMatrix();
	const Vector3 nodePosW = *(Vector3*)&bbox.m_bbox.Center;

	vector<cTile*> candidate;
	for (auto &tile : m_tiles)
	{
		cBoundingBox tbbox = tile->m_boundingBox * tile->GetWorldMatrix();
		if (tbbox.Collision(bbox))
			candidate.push_back(tile);
	}

	RETV(candidate.empty(), false);

	float nearLen = FLT_MAX;
	cTile *nearTile = NULL;
	for (auto &tile : candidate)
	{
		const float len = tile->GetWorldMatrix().GetPosition().Distance(nodePosW);
		if (len < nearLen)
		{
			nearLen = len;
			nearTile = tile;
		}
	}

	return nearTile;
}


void cTerrain::SetShadowRendering(const bool isRender)
{
	for (auto &p : m_tiles)
		p->SetRenderFlag(eRenderFlag::SHADOW, isRender);
}


inline float Lerp(float p1, float p2, float alpha)
{
	return p1 * (1.f - alpha) + p2 * alpha;
}


// x/z평면에서 월드 좌표 x,z 위치에 해당하는 높이 값 y를 리턴한다.
float cTerrain::GetHeight(const float x, const float z)
{
	const float newX = x / m_cellSize.x;
	const float newZ = z / m_cellSize.y;

	const float col = ::floorf(newX);
	const float row = ::floorf(newZ);

	//  A   B
	//  *---*
	//  | / |
	//  *---*
	//  C   D
	const float A = GetHeightMapEntry((int)row, (int)col);
	const float B = GetHeightMapEntry((int)row, (int)col + 1);
	const float C = GetHeightMapEntry((int)row + 1, (int)col);
	const float D = GetHeightMapEntry((int)row + 1, (int)col + 1);

	const float dx = newX - col;
	const float dz = newZ - row;

	float height = 0.0f;
	if (dz < 1.0f - dx)  // upper triangle ABC
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C
		height = A + Lerp(0.0f, uy, dx) + Lerp(0.0f, vy, dz);
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B
		height = D + Lerp(0.0f, uy, 1.0f - dx) + Lerp(0.0f, vy, 1.0f - dz);
	}

	return height;
}


// 피킹된 지형 좌표를 리턴한다.
// ray.orig에서 가장 가까운 면에 피킹된 좌표를 리턴한다.
// 지형 범위를 넘어섰다면, y = 0 가된다.
// 최적화: 타일 충돌체크 후, 충돌된 타일만 충돌 체크를 한다.
Vector3 cTerrain::GetHeightFromRay(const Ray &ray)
{
	const int cellCountPerTile_Col = m_cols / m_tileCols;
	const int cellCountPerTile_Row = m_rows / m_tileRows;

	Vector3 candidate[8];
	int count = 0;

	bool isFirst = true;
	for (auto &tile : m_tiles)
	{
		cBoundingBox bbox = tile->m_boundingBox;
		bbox *= tile->GetWorldMatrix();
		if (!bbox.Pick(ray))
			continue;

		const int startCol = tile->m_location.y * cellCountPerTile_Col;
		const int startRow = tile->m_location.x * cellCountPerTile_Row;

		for (int r = startRow; r < startRow + cellCountPerTile_Row; ++r)
		{
			for (int c = startCol; c < startCol + cellCountPerTile_Col; ++c)
			{
				const int indices[2][3] = {
					{ r*m_colVtx + c,  (r + 1)*m_colVtx + c, r*m_colVtx + c + 1 }
					, { (r + 1)*m_colVtx + c,  (r + 1)*m_colVtx + c + 1, r*m_colVtx + c + 1 }
				};

				for (int i = 0; i < 2; ++i)
				{
					const Vector3 p1 = m_heightMap[indices[i][0]].p;
					const Vector3 p2 = m_heightMap[indices[i][1]].p;
					const Vector3 p3 = m_heightMap[indices[i][2]].p;

					const Triangle tri(p1, p2, p3);
					const Plane p(p1, p2, p3);
					const float dot = ray.dir.DotProduct(p.N);
					if (dot >= 0)
						continue;

					float t;
					if (tri.Intersect(ray.orig, ray.dir, &t))
					{
						Vector3 pos = ray.orig + ray.dir * t;
						if (count < ARRAYSIZE(candidate))
							candidate[count++] = pos;
						else
							break;
					}
				}

			}
		}

	}

	if (count == 1)
		return candidate[0];

	if (count == 0)
	{
		// 지형과 충돌이 일어나지 않았다면, X-Z 평면의 충돌지점을 리턴한다.
		Plane ground(Vector3(0, 1, 0), 0);
		return ground.Pick(ray.orig, ray.dir);
	}

	// 하나 이상의 면과 충돌이 일어났다면, Ray.orig 와 가까운 위치를 리턴한다.
	float minLen = candidate[0].LengthRoughly(ray.orig);
	int retIdx = 0;
	for (int i = 1; i < count; ++i)
	{
		const float len = candidate[i].LengthRoughly(ray.orig);
		if (minLen > len)
		{
			minLen = len;
			retIdx = i;
		}
	}

	return candidate[retIdx];
}


// 맵을 2차원 배열로 봤을 때, row, col 인덱스의 높이 값을 리턴한다.
// 맵 범위를 벗어 났을 때, m_defaultHeight 값을 보간해서 리턴한다.
float cTerrain::GetHeightMapEntry(int row, int col)
{
	if (0 > row || 0 > col)
	{
		const int r = max(min(row, m_rows - 1), 0);
		const int c = max(min(col, m_cols - 1), 0);
		const float a = (float)sqrt((r - row)*(r - row) + (c - col)*(c - col)) * m_lerpAlphaFactor;

		const float h = GetHeightMapEntry(r, c);
		return lerp(h, m_defaultHeight, 0);
	}

	if (m_rows <= row || m_cols <= col)
	{
		const int r = max(min(row, m_rows - 1), 0);
		const int c = max(min(col, m_cols - 1), 0);
		const float a = (float)sqrt((r-row)*(r-row) + (c-col)*(c-col)) * m_lerpAlphaFactor;

		const float h = GetHeightMapEntry(r,c);
		return lerp(h, m_defaultHeight, 0);
	}
	if ((int)m_heightMap.size() <= (row * m_colVtx + col))
		return m_defaultHeight;

	return m_heightMap[row*m_colVtx + col].p.y;
}


void cTerrain::Clear()
{
	__super::Clear();
	m_tiles.clear();
	m_tilemap.clear();
	m_tilemap2.clear();
}


// Normalize All HeightMap
void cTerrain::HeightmapNormalize()
{
	const float radius = sqrt((float)(m_cols*m_cols + m_rows*m_rows)) * m_cellSize.Length();
	HeightmapNormalize(Vector3(0, 0, 0), radius);
}


void cTerrain::HeightmapNormalize(const Vector3 &cursorPos, const float radius)
{
	cTerrain &terrain = *this;

	const float x = ((cursorPos.x - radius) / m_cellSize.x) - 2;
	const float z = ((cursorPos.z - radius) / m_cellSize.y) - 2;
	const int startX = (int)max(0, x);
	const int startZ = (int)max(0, z);
	const int rowSize = (int)((radius * 2) / m_cellSize.y) + 4;
	const int colSize = (int)((radius * 2) / m_cellSize.x) + 4;
	const int endZ = min(startZ + rowSize, m_rowVtx);
	const int endX = min(startX + colSize, m_colVtx);

	// Update Vertices Normal
	for (int r = startZ; r <endZ; ++r)
	{
		for (int c = startX; c < endX; ++c)
		{
			Vector3 n(0, 0, 0);
			Vector3 p0 = terrain.m_heightMap[r*terrain.m_colVtx + c].p;

			//   z (row)
			//   |
			//   |
			//   |
			//   |
			//  -|--------------> x (col)
			//  Axis X-Z
			//
			//
			// (r+1,c-1)
			// * ---------- * ----------- * (r+1, c+1)
			// |          / |           / |
			// |       /    |        /    |
			// |    /       |     /       |
			// | /          | (r,c)       | (r,c+1)
			// * ---------- * ----------- *
			// |         /  |          /  |
			// |       /    |        /    |
			// |    /       |     /       |
			// | /          |   /         |
			// * ---------- * ----------- * (r-1,c+1)
			// (r-1,c-1)
			//

			// left top triangle 1
			if ((r + 1 < terrain.m_rowVtx) && (c - 1 >= 0))
			{
				Vector3 p1 = terrain.m_heightMap[r*terrain.m_colVtx + (c - 1)].p;
				Vector3 p2 = terrain.m_heightMap[(r + 1)*terrain.m_colVtx + c].p;
				Triangle t(p0, p1, p2);
				n += t.Normal();
			}

			// right top triangle 1
			if ((r + 1 < terrain.m_rowVtx) && (c + 1 < terrain.m_colVtx))
			{
				Vector3 p1 = terrain.m_heightMap[(r + 1)*terrain.m_colVtx + c].p;
				Vector3 p2 = terrain.m_heightMap[(r + 1)*terrain.m_colVtx + (c + 1)].p;
				Triangle t(p0, p1, p2);
				n += t.Normal();
			}

			// right top triangle 2
			if ((r + 1 < terrain.m_rowVtx) && (c + 1 < terrain.m_colVtx))
			{
				Vector3 p1 = terrain.m_heightMap[(r + 1)*terrain.m_colVtx + (c + 1)].p;
				Vector3 p2 = terrain.m_heightMap[r*terrain.m_colVtx + (c + 1)].p;
				Triangle t(p0, p1, p2);
				n += t.Normal();
			}


			// right bottom triangle 1
			if ((r - 1 >= 0) && (c + 1 < terrain.m_colVtx))
			{
				Vector3 p1 = terrain.m_heightMap[r*terrain.m_colVtx + (c + 1)].p;
				Vector3 p2 = terrain.m_heightMap[(r - 1)*terrain.m_colVtx + c].p;
				Triangle t(p0, p1, p2);
				n += t.Normal();
			}

			// left bottom triangle 1
			if ((r - 1 >= 0) && (c - 1 >= 0))
			{
				Vector3 p1 = terrain.m_heightMap[(r - 1)*terrain.m_colVtx + c].p;
				Vector3 p2 = terrain.m_heightMap[(r - 1)*terrain.m_colVtx + (c - 1)].p;
				Triangle t(p0, p1, p2);
				n += t.Normal();
			}

			// left bottom triangle 2
			if ((r - 1 >= 0) && (c - 1 >= 0))
			{
				Vector3 p1 = terrain.m_heightMap[(r - 1)*terrain.m_colVtx + (c - 1)].p;
				Vector3 p2 = terrain.m_heightMap[r*terrain.m_colVtx + (c - 1)].p;
				Triangle t(p0, p1, p2);
				n += t.Normal();
			}

			//n /= (float)cnt;
			n.Normalize();
			terrain.m_heightMap[r*terrain.m_colVtx + c].n = n;
		}
	}
}


void cTerrain::UpdateHeightmapToTile(cRenderer &renderer, cTile *tiles[], const int tileCount)
{
	// Initialize Map
	// 
	//   z (row)
	//   |
	//   |
	//   |
	//   |
	//  -|--------------> x (column)
	//  Axis X-Z
	//
	//
	// 8      9      10      11
	// * ---- * ---- * ---- *
	// |      |      |      |
	// |4     | 5    |6     |7
	// * ---- * ---- * ---- *
	// |      |      |      |
	// |0     | 1    | 2    |3
	// * ---- * ---- * ---- *
	//
	// Terrain Vertex Store Order
	//

	//
	// + --------------- + --------------- + --------------- +
	// |                 |                 |                 |
	// | tile(r=2,c=0)   | tile(r=2,c=1)   |  tile(r=2,c=2)  |
	// |                 |                 |                 |
	// + --------------- + --------------- + --------------- +
	// |                 |                 |                 |
	// | tile(r=1,c=0)   | tile(r=1,c=1)   |  tile(r=1,c=2)  |
	// |                 |                 |                 |
	// + --------------- + --------------- + --------------- +
	// |                 |                 |                 |
	// | tile(r=0,c=0)   | tile(r=0,c=1)   |  tile(r=0,c=2)  |
	// |                 |                 |                 |
	// + --------------- + --------------- + --------------- +
	//
	// Tile Row-Col Axis
	//

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
	// Grid Vertex Store Order
	//

	cTerrain &terrain = *this;
	const int cellCountPerTile_Col = terrain.m_cols / terrain.m_tileCols;
	const int cellCountPerTile_Row = terrain.m_rows / terrain.m_tileRows;
	const int vertexCountPerTile_Col = (terrain.m_cols / terrain.m_tileCols) + 1;
	const int vertexCountPerTile_Row = (terrain.m_rows / terrain.m_tileRows) + 1;

	for (int i = 0; i < tileCount; ++i)
	{
		cTile *tile = tiles[i];
		const int mapStartIdx = (tile->m_location.x * cellCountPerTile_Row * terrain.m_colVtx)
			+ (tile->m_location.y * cellCountPerTile_Col)
			+ (cellCountPerTile_Row * terrain.m_colVtx); // 왜냐하면, 그리드 좌표계와 지형 좌표계 row 가 반대이기 때문임.

		// all tiles must be same size, same vertex size
		if (!m_cpyVtxBuff.m_buff)
			m_cpyVtxBuff.Create(renderer, tile->m_ground->m_vtxBuff);

		if (!m_cpyVtxBuff.CopyFrom(renderer, tile->m_ground->m_vtxBuff))
			break;

		cVertexBuffer &dstVb = tile->m_ground->m_vtxBuff;
		sVertexNormTex *src = (sVertexNormTex*)m_cpyVtxBuff.Lock(renderer);
		sVertexNormTex *dst = (sVertexNormTex*)dstVb.Lock(renderer);

		const Matrix44 invTm = tile->GetWorldMatrix().Inverse();
		for (int r = 0; r < vertexCountPerTile_Row; ++r)
		{
			for (int c = 0; c < vertexCountPerTile_Col; ++c)
			{
				const int mapIdx = mapStartIdx - (r * terrain.m_colVtx) + c;
				const int srcIdx = (r * vertexCountPerTile_Col) + c;
				dst[srcIdx] = src[srcIdx];
				dst[srcIdx].p = terrain.m_heightMap[mapIdx].p * invTm; // 타일 로컬좌표로 바꾼다.
				dst[srcIdx].n = terrain.m_heightMap[mapIdx].n;
			}
		}

		m_cpyVtxBuff.Unlock(renderer);
		dstVb.Unlock(renderer);
	}
}


// 입력으로 leftTop, RightBottom 좌표를 주면, X-Z 평면을 기준으로한
// 사각영역의 라인 좌표를 만들어서 리턴한다. (cRect3D)
// 버텍스는 한 변당 32개이고, 총 128개의 버텍스를 사용한다.
// p0 : X-Z Plane Left Top
// p1 : X-Z Plane Right Bottom
// maxEdgeVertexCount = min(32, maxEdgeVertexCount)
void cTerrain::GetRect3D(cRenderer &renderer
	, const Vector3 &p0, const Vector3 &p1
	, OUT cRect3D &out
	, const int maxEdgeVertexCount //=32
	, const Vector3 &offset //= Vector3(0, 0.15f, 0)
	)
{
	// X-Z Plane
	Vector3 patch[4]; // leftop, righttop, rightbottom, leftbottom
	patch[0] = p0;
	patch[1] = Vector3(p1.x, p0.y, p0.z);
	patch[2] = p1;
	patch[3] = Vector3(p0.x, p0.y, p1.z);

	assert(maxEdgeVertexCount == 32);
	//const int vtxSize = (maxEdgeVertexCount < 32) ? maxEdgeVertexCount : 32;
	const int vtxSize = 32;
	int indices[5] = { 0,1,2,3,0 };
	Vector3 buff[vtxSize * 4];
	int idx = 0;
	for (int i = 0; i < 4; ++i)
	{
		Vector3 start = patch[indices[i]];
		Vector3 end = patch[indices[i+1]];

		Vector3 v = end - start;
		const float inc = v.Length() / (float)(vtxSize-1);
		v.Normalize();
		
		for (int k = 0; k < vtxSize; ++k)
		{
			const Vector3 p = start + v*(inc*(float)k);
			Vector3 pos = p;
			pos.y = GetHeight(p.x, p.z);
			buff[idx++] = pos + offset;
		}
	}

	out.SetRect(renderer, buff, ARRAYSIZE(buff));
}
