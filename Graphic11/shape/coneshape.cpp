
#include "stdafx.h"
#include "coneshape.h"

using namespace graphic;


cConeShape::cConeShape()
{
}

cConeShape::~cConeShape()
{
}


bool cConeShape::Create(cRenderer &renderer, const float radius, const float height
	, const int slices
	, const int vtxType // = (eVertexType::POSITION | eVertexType::NORMAL| eVertexType::COLOR)
	, const cColor &color // = cColor::BLACK
)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return false; // already exist

	m_vtxType = vtxType;

	const int vtxCount = slices * 3;

	vector<Vector3> coneVertices;
	coneVertices.reserve(vtxCount);
	float dth = MATH_PI*2.f / slices;
	for (int i = 0; i < slices; ++i)
	{
		const float th0 = dth * i;
		const float th1 = (i==slices-1)? (MATH_PI*2) : dth * (i + 1);

		const float x0 = (float)cos(th0) * radius;
		const float z0 = (float)sin(th0) * radius;
		const float x1 = (float)cos(th1) * radius;
		const float z1 = (float)sin(th1) * radius;
		coneVertices.push_back(Vector3(0, height, 0));
		coneVertices.push_back(Vector3(x0, 0, z0));
		coneVertices.push_back(Vector3(x1, 0, z1));
	}

	const Vector4 vColor = color.GetColor();

	cVertexLayout vtxLayout;
	vtxLayout.Create(vtxType);
	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();

	vector<BYTE> vertices(vtxCount * vertexStride);
	BYTE *pVertex = &vertices[0];

	for (uint i = 0; i < coneVertices.size(); ++i)
	{
		Vector3 pos = coneVertices[i];

		if (vtxType & eVertexType::POSITION)
			*(Vector3*)(pVertex + posOffset) = pos;
		if (vtxType & eVertexType::COLOR)
			*(Vector4*)(pVertex + colorOffset) = vColor;
		if (vtxType & eVertexType::NORMAL)
			*(Vector3*)(pVertex + normOffset) = pos.Normal();
		//if (vtxType & eVertexType::TEXTURE0)
		//	*(Vector2*)(pVertex + texOffset) = Vector2(u1, v1);

		pVertex += vertexStride;
	}

	m_vtxBuff.Create(renderer, vtxCount, vertexStride, &vertices[0]);
	return true;
}


void cConeShape::Render(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	renderer.GetDevContext()->Draw(m_vtxBuff.GetVertexCount(), 0);
}

