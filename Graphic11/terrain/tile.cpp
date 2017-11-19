#include "stdafx.h"
#include "tile.h"

using namespace graphic;


cTile::cTile()
	: cNode(common::GenerateId(), "tile", eNodeType::TERRAIN)
	, m_isDbgRender(false)
	, m_location(-1,-1)
	, m_isHilight(false)
{
}

cTile::~cTile()
{
	Clear();
}


//
// rect =  3D-axis
//
//  z (rect.top, bottom)
//  |
//  |
//  |
//  + ---------> x (rect.left, right)
// 
//
//   rect.left, top (X-Z axis)  
//   * ---------------------------------- * rect.right, top (X-Z axis)
//   |                                    |
//   |                                    |
//   |                 +                  |
//   |               center               |
//   |                                    |
//   * ---------------------------------- * rect.right, bottom (X-Z axis)
//   rect.left, bottom (X-Z axis)
//
//
bool cTile::Create(cRenderer &renderer
	, const int id
	, const Str64 &name
	, const int row  // tile row index from terrain
	, const int col  // tile column index from terrain
	, const sRectf &rect // (x-z axis)
	, const char *textureFileName //= g_defaultTexture
	, const float uvFactor //= 1.f,
	, const Vector2 &uv0 //= Vector2(0, 0)
	, const Vector2 &uv1 //= Vector2(1, 1)
)
{
	m_id = id;
	m_name = name;
	m_location = Vector2i(row, col);
	SetRenderFlag(eRenderFlag::SHADOW, true);

	const float cellSize = rect.Width() / 2.f;
	m_ground = new cGrid();
	m_ground->Create(renderer, 16, 16, cellSize/8.f
		, (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE)
		, cColor::WHITE, textureFileName, uv0, uv1, uvFactor
		, true);
	m_ground->m_renderFlags = eRenderFlag::VISIBLE | eRenderFlag::TERRAIN | eRenderFlag::SHADOW;
	m_ground->SetOpFlag(eOpFlag::COLLISION, false);
	m_ground->m_isLineDrawing = true;
	//m_ground->m_transform.pos.y = -3.f;
	if (!textureFileName)
		m_ground->m_mtrl.InitWhite();
	AddChild(m_ground);
	
	m_transform.pos = Vector3(rect.left + cellSize, 0, rect.top + cellSize);
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(cellSize, cellSize, cellSize), Quaternion());
	CalcBoundingSphere();
	//m_boundingBox.SetBoundingBox( Vector3(0,0,0), Vector3(cellSize, 20000, cellSize), Quaternion());

	return true;
}


bool cTile::Update(cRenderer &renderer, const float deltaSeconds)
{
	return __super::Update(renderer, deltaSeconds);
}


// 쉐도우 맵에 그리기위한 렌더 함수
// 쉐도우 플래그가 있는 모델들만 그린다.
void cTile::PreRender(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	RET(!(m_renderFlags & eRenderFlag::SHADOW));
	RET(!m_isEnable);
	RET(!IsVisible());

	const XMMATRIX transform = m_transform.GetMatrixXM() * tm;
	for (auto &p : m_children)
	{
		if (p->m_renderFlags & eRenderFlag::SHADOW)
			p->Render(renderer, transform, eRenderFlag::SHADOW);
	}
}


bool cTile::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);

	if ((flags & 0x1) && m_isDbgRender)
		DebugRender(renderer, tm);

	if (m_isHilight)
	{
		const XMMATRIX transform = m_transform.GetMatrixXM() * tm;
		renderer.m_dbgBox.SetBox(m_boundingBox);
		renderer.m_dbgBox.Render(renderer, transform);
	}

	return __super::Render(renderer, tm, flags);
}


void cTile::DebugRender(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	const XMMATRIX transform = m_transform.GetMatrixXM() * tm;
	renderer.m_dbgBox.SetBox(m_boundingBox);
	renderer.m_dbgBox.Render(renderer, transform);
}


// return frustum center to tile length
float cTile::CullingTest(const cFrustum &frustum
	, const XMMATRIX &tm //= XMIdentity
	, const bool isModel //= true
)
{
	RETV(!m_isEnable, false);

	const XMMATRIX transform = m_transform.GetMatrixXM()*tm;
	cBoundingSphere bsphere = m_boundingSphere * transform;
	if (frustum.IsInSphere(bsphere))
	{
		SetRenderFlag(eRenderFlag::VISIBLE, true);

		if (isModel)
		{
			for (auto &p : m_children)
				p->CullingTest(frustum, transform, isModel);
		}
		else
		{
			for (auto &p : m_children)
				p->SetRenderFlag(eRenderFlag::VISIBLE, true);
		}
	}
	else
	{
		SetRenderFlag(eRenderFlag::VISIBLE, false);
	}

	return frustum.LengthRoughly(m_boundingBox.Center() * transform);
}


bool cTile::AddChild(cNode *node)
{
	return __super::AddChild(node);
}


bool cTile::RemoveChild(cNode *rmNode
	, const bool rmInstance //= true
)
{
	return __super::RemoveChild(rmNode, rmInstance);
}


cNode* cTile::FindNode(const int id) 
{
	return __super::FindNode(id);
}


void cTile::UpdatePosition(const sRectf &rect)
{
	const float cellSize = rect.Width() / 2.f;
	m_transform.pos = Vector3(rect.left + cellSize, 0, rect.top + cellSize);
	m_boundingBox.SetBoundingBox(Vector3(0, 10, 0), Vector3(cellSize, 20, cellSize), Quaternion());
	CalcBoundingSphere();
}


void cTile::Clear()
{
}
