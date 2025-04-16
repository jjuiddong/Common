
#include "stdafx.h"
#include "quadshape.h"

using namespace graphic;


cQuadShape::cQuadShape()
	: m_width(0)
	, m_height(0)
{
}

cQuadShape::cQuadShape(cRenderer &renderer
	, const int vtxType //= (eVertexType::POSITION | eVertexType::TEXTURE0)
	, const cColor &color //= cColor::WHITE
	, const ePlaneType planeType //= ePlaneType::XY
)
	: m_width(0)
	, m_height(0)
{
	Create(renderer, vtxType, color, 2, 2, false, planeType);
}


cQuadShape::~cQuadShape()
{
}


bool cQuadShape::Create(cRenderer &renderer
	, const int vtxType //= (eVertexType::POSITION | eVertexType::TEXTURE0)
	, const cColor &color //= cColor::WHITE
	, const float width //= 2
	, const float height //= 2
	, const bool isDynamic //= false
	, const ePlaneType planeType //= ePlaneType::XY
)
{
	const Vector3 vertices_xy[4] = {
		Vector3(-width / 2.f, height / 2.f, 0),
		Vector3(width / 2.f, height / 2.f, 0),
		Vector3(-width / 2.f,-height / 2.f, 0),
		Vector3(width / 2.f,-height / 2.f, 0),
	};

	const Vector3 vertices_xz[4] = {
		Vector3(-width / 2.f, 0, height / 2.f),
		Vector3(width / 2.f, 0, height / 2.f),
		Vector3(-width / 2.f, 0, -height / 2.f),
		Vector3(width / 2.f, 0, -height / 2.f),
	};

	Vector2 uv[4] = {
		Vector2(0,0)
		, Vector2(1,0)
		, Vector2(0,1)
		, Vector2(1,1)
	};

	m_vtxLayout.Create(vtxType);
	const int posOffset = m_vtxLayout.GetOffset("POSITION");
	const int normOffset = m_vtxLayout.GetOffset("NORMAL");
	const int colorOffset = m_vtxLayout.GetOffset("COLOR");
	const int texOffset = m_vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = m_vtxLayout.GetVertexSize();
	vector<BYTE> buffer(vertexStride * 24);
	BYTE *initVertices = &buffer[0];

	const Vector3 *vertices = (planeType == ePlaneType::XY) ? vertices_xy : vertices_xz;
	const Vector4 vColor = color.GetColor();
	BYTE *pvtx = initVertices;
	for (int i = 0; i < 4; ++i)
	{
		if ((vtxType & eVertexType::POSITION) || (vtxType & eVertexType::POSITION_RHW))
			*(Vector3*)(pvtx + posOffset) = vertices[i];
		if (vtxType & eVertexType::NORMAL)
			*(Vector3*)(pvtx + normOffset) = Vector3(0,0,-1.f);
		if (vtxType & eVertexType::COLOR)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		if (vtxType & eVertexType::TEXTURE0)
			*(Vector2*)(pvtx + texOffset) = uv[i];
		pvtx += vertexStride;
	}

	m_vtxBuff.Create(renderer, 4, vertexStride, initVertices, isDynamic? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT);
	m_vtxType = vtxType;
	m_width = width;
	m_height = height;

	return true;
}


void cQuadShape::Render(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	renderer.GetDevContext()->DrawInstanced(4, 1, 0, 0);
}


void cQuadShape::RenderInstancing(cRenderer & renderer, const int count)
{
	m_vtxBuff.Bind(renderer);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	renderer.GetDevContext()->DrawInstanced(4, count, 0, 0);
}


// Set UV Position
// Vector2.xy = {u , v}
// lt : left top
// rt : right top
// lb : left bottom
// rb : right bottom
void cQuadShape::SetUV(cRenderer &renderer, const Vector2 &lt, const Vector2 &rt, const Vector2 &lb, const Vector2 &rb)
{
	RET(m_vtxBuff.GetVertexCount() <= 0);

	Vector3 vertices[4] = {
		Vector3(-m_width / 2.f, m_height / 2.f, 0),
		Vector3(m_width / 2.f, m_height / 2.f, 0),
		Vector3(-m_width / 2.f,-m_height / 2.f, 0),
		Vector3(m_width / 2.f,-m_height / 2.f, 0),
	};

	Vector2 uv[4] = {lt, rt, lb, rb};

	const int posOffset = m_vtxLayout.GetOffset("POSITION");
	const int normOffset = m_vtxLayout.GetOffset("NORMAL");
	const int colorOffset = m_vtxLayout.GetOffset("COLOR");
	const int texOffset = m_vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = m_vtxLayout.GetVertexSize();

	BYTE *pvtx = (BYTE*)m_vtxBuff.Lock(renderer);
	RET(!pvtx);

	Vector3 *v0 = (Vector3*)(pvtx + (vertexStride * 0) + posOffset);
	Vector3 *v1 = (Vector3*)(pvtx + (vertexStride * 1) + posOffset);
	Vector3 *v2 = (Vector3*)(pvtx + (vertexStride * 2) + posOffset);
	Vector3 *v3 = (Vector3*)(pvtx + (vertexStride * 3) + posOffset);

	Vector2 *uv0 = (Vector2*)(pvtx + (vertexStride * 0) + texOffset);
	Vector2 *uv1 = (Vector2*)(pvtx + (vertexStride * 1) + texOffset);
	Vector2 *uv2 = (Vector2*)(pvtx + (vertexStride * 2) + texOffset);
	Vector2 *uv3 = (Vector2*)(pvtx + (vertexStride * 3) + texOffset);

	// left top
	*v0 = vertices[0];
	*uv0 = lt;
	// right top
	*v1 = vertices[1];
	*uv1 = rt;
	// left bottom
	*v2 = vertices[2];
	*uv2 = lb;
	// right bottom
	*v3 = vertices[3];
	*uv3 = rb;

	m_vtxBuff.Unlock(renderer);
}
