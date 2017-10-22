
#include "stdafx.h"
#include "terrain2.h"
#include "tile.h"


using namespace graphic;


cTerrain2::cTerrain2()
	: cNode(common::GenerateId(), "terrain", eNodeType::TERRAIN)
	, m_isShowDebug(false)
	, m_rows(0)
	, m_cols(0)
	, m_tileCols(0)
	, m_tileRows(0)
{
}

cTerrain2::~cTerrain2()
{
	Clear();
}


bool cTerrain2::Create(cRenderer &renderer, const sRectf &rect)
{
	const Vector2 center = rect.Center();
	const Vector3 lightLookat = Vector3(center.x, 0, center.y);
	const Vector3 lightPos = GetMainLight().m_pos;
	const Vector3 p0 = lightPos;
	const Vector3 p1 = (lightLookat - lightPos).Normal() * 3 + p0;
	m_dbgLightDir.Create(renderer, p0, p1, 0.5F);

	m_dbgPlane.Create(renderer);
	m_dbgPlane.SetLine(Vector3(0, 0, 0), Vector3(0, 30, 0), 0.1f);

	return true;
}


bool cTerrain2::Create(cRenderer &renderer, const int rowCnt, const int colCnt, const float cellSize
	, const int rowTileCnt, const int colTileCnt)
{
	m_rows = rowCnt;
	m_cols = colCnt;
	m_cellSize = cellSize;
	m_rowVtx = rowCnt + 1;
	m_colVtx = colCnt + 1;
	m_tileCols = colTileCnt;
	m_tileRows = rowTileCnt;
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
			Vector3 p(c*cellSize, 0, r*cellSize);
			Vector3 n(0, 1, 0);
			const int idx = r*(colCnt+1) + c;
			m_heightMap[idx].p = p;
			m_heightMap[idx].n = n;
		}
	}

	return true;
}


bool cTerrain2::Update(cRenderer &renderer, const float deltaSeconds)
{
	__super::Update(renderer, deltaSeconds);
	return true;
}


void cTerrain2::BuildCascadedShadowMap(cRenderer &renderer
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
			p->PreRender(renderer, tm);
		ccsm.End(renderer, i);
	}

	CullingTestOnly(renderer, GetMainCamera()); // Recovery Culling
}


bool cTerrain2::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	CullingTestOnly(renderer, GetMainCamera()); // Recovery Culling
	return __super::Render(renderer, tm);
}


bool cTerrain2::RenderCascadedShadowMap(cRenderer &renderer
	, cCascadedShadowMap &ccsm
	, const XMMATRIX &tm //= XMIdentity
	, const int flags // =1
)
{
	ccsm.Bind(renderer);
	__super::Render(renderer, tm);

	if (m_isShowDebug)
		ccsm.DebugRender(renderer);

	return true;
}


void cTerrain2::RenderOption(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int option //= 0x1
)
{
	__super::Render(renderer, tm, option);
}


void cTerrain2::RenderDebug(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	if (m_isShowDebug)
	{
		m_dbgLightDir.Render(renderer);
		m_dbgPlane.Render(renderer);
	}
}


void cTerrain2::CullingTestOnly(cRenderer &renderer, cCamera &camera
	, const bool isModel //= true
)
{
	cFrustum frustum;
	frustum.SetFrustum(camera.GetViewProjectionMatrix());

	for (auto &p : m_tiles)
		p->CullingTest(frustum, XMIdentity, isModel);
}


bool cTerrain2::AddTile(cTile *tile)
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


bool cTerrain2::RemoveTile(cTile *tile)
{
	RemoveChild(tile);

	auto it = std::find(m_tiles.begin(), m_tiles.end(), tile);
	if (m_tiles.end() == it)
		return false; // Not Exist

	common::popvector2(m_tiles, tile);
	m_tilemap.erase(tile->m_name.GetHashCode());
	m_tilemap2.erase(tile->m_id);
	return true;
}


// insert model to most nearest tile
bool cTerrain2::AddModel(cNode *model)
{
	cTile *nearTile = GetNearestTile(model);
	assert(nearTile);

	model->m_transform = model->m_transform * nearTile->m_transform.Inverse();
	model->CalcBoundingSphere();

	return nearTile->AddChild(model);
}


// Update Model Position
bool cTerrain2::UpdateModel(cNode *model)
{
	cTile *nearTile = GetNearestTile(model);
	assert(nearTile);

	Transform transform = model->GetWorldTransform();
	model->m_transform = transform * nearTile->m_transform.Inverse();
	model->CalcBoundingSphere();

	if (model->m_parent)
		model->m_parent->RemoveChild(model, false);

	return nearTile->AddChild(model);
}


cTile* cTerrain2::GetNearestTile(const cNode *node)
{
	cBoundingBox bbox = node->m_boundingBox;
	bbox *= node->GetWorldMatrix();
	//Transform transform = node->m_transform;
	//bbox.SetBoundingBox(transform.pos, transform.scale, transform.rot);

	vector<cTile*> candidate;
	for (auto &tile : m_tiles)
	{
		cBoundingBox tbbox = tile->m_boundingBox * tile->GetWorldMatrix();
		if (tbbox.Collision(bbox))
			candidate.push_back(tile);
	}

	RETV2(candidate.empty(), false);

	float nearLen = FLT_MAX;
	cTile *nearTile = NULL;
	for (auto &tile : candidate)
	{
		const float len = tile->GetWorldMatrix().GetPosition().Distance(node->m_transform.pos);
		if (len < nearLen)
		{
			nearLen = len;
			nearTile = tile;
		}
	}

	return nearTile;
}


void cTerrain2::SetDbgRendering(const bool isRender)
{
	m_isShowDebug = isRender;

	for (auto &p : m_tiles)
	{
		p->m_isDbgRender = isRender;

		//for (auto &ch : p->m_children)
		//	((cTile*)ch)->m_isDbgRender = isRender;
	}
}


void cTerrain2::SetShadowRendering(const bool isRender)
{
	for (auto &p : m_tiles)
		p->SetRenderFlag(eRenderFlag::SHADOW, isRender);
}


inline float Lerp(float p1, float p2, float alpha)
{
	return p1 * (1.f - alpha) + p2 * alpha;
}


// x/z평면에서 월드 좌표 x,z 위치에 해당하는 높이 값 y를 리턴한다.
float cTerrain2::GetHeight(const float x, const float z)
{
	const float newX = x / m_cellSize;
	const float newZ = z / m_cellSize;

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


bool cTerrain2::GetHeightFromRay(const Ray &ray, OUT Vector3 &out)
{
	bool isFirst = true;
	for (int r = 0; r < m_rows; ++r)
	{
		for (int c = 0; c < m_cols; ++c)
		{
			const int indices[2][3] = {
				{ r*m_colVtx + c,  (r + 1)*m_colVtx + c, r*m_colVtx + c + 1 }
				, { (r + 1)*m_colVtx + c,  (r + 1)*m_colVtx + c + 1, r*m_colVtx + c+1 }
			};

			for (int i = 0; i < 2; ++i)
			{
				const Vector3 p1 = m_heightMap[ indices[i][0]].p;
				const Vector3 p2 = m_heightMap[ indices[i][1]].p;
				const Vector3 p3 = m_heightMap[ indices[i][2]].p;
		
				const Triangle tri(p1, p2, p3);
				const Plane p(p1, p2, p3);
				const float dot = ray.dir.DotProduct(p.N);
				if (dot >= 0)
					continue;

				float t;
				if (tri.Intersect(ray.orig, ray.dir, &t))
				{
					if (isFirst)
					{
						isFirst = false;
						out = ray.orig + ray.dir * t;
					}
					else
					{
						const Vector3 v = ray.orig + ray.dir * t;
						if (ray.orig.LengthRoughly(v) < ray.orig.LengthRoughly(out))
							out = v;
					}
				}
			}

		}
	}

	return !isFirst;
}


// 맵을 2차원 배열로 봤을 때, row, col 인덱스의 높이 값을 리턴한다.
float cTerrain2::GetHeightMapEntry(int row, int col)
{
	if (0 > row || 0 > col)
		return 0.f;
	if ((int)m_heightMap.size() <= (row * m_colVtx + col))
		return 0.f;

	return m_heightMap[row*m_colVtx + col].p.y;
}


void cTerrain2::Clear()
{
	__super::Clear();
	m_tiles.clear();
	m_tilemap.clear();
	m_tilemap2.clear();
}


// Normalize All HeightMap
void cTerrain2::HeightmapNormalize()
{
	const float radius = sqrt((float)(m_cols*m_cols + m_rows*m_rows)) * m_cellSize;
	HeightmapNormalize(Vector3(0, 0, 0), radius);
}


void cTerrain2::HeightmapNormalize(const Vector3 &cursorPos, const float radius)
{
	cTerrain2 &terrain = *this;

	const float x = ((cursorPos.x - radius) / m_cellSize) - 2;
	const float z = ((cursorPos.z - radius) / m_cellSize) - 2;
	const int startX = (int)max(0, x);
	const int startZ = (int)max(0, z);
	const int rowSize = (int)((radius * 2) / m_cellSize) + 4;
	const int colSize = (int)((radius * 2) / m_cellSize) + 4;
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


void cTerrain2::UpdateHeightmapToTile(cRenderer &renderer, cTile *tiles[], const int tileCount)
{
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

	cTerrain2 &terrain = *this;
	const int cellCountPerTile_Col = terrain.m_cols / terrain.m_tileCols;
	const int cellCountPerTile_Row = terrain.m_rows / terrain.m_tileRows;
	const int vertexCountPerTile_Col = (terrain.m_cols / terrain.m_tileCols) + 1;
	const int vertexCountPerTile_Row = (terrain.m_rows / terrain.m_tileRows) + 1;

	//for (auto tile : tiles)
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
				dst[srcIdx].p = terrain.m_heightMap[mapIdx].p * invTm;
				dst[srcIdx].n = terrain.m_heightMap[mapIdx].n;
			}
		}

		m_cpyVtxBuff.Unlock(renderer);
		dstVb.Unlock(renderer);
	}
}
