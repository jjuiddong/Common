
#include "stdafx.h"
#include "pyramidshape.h"


using namespace graphic;


cPyramidShape::cPyramidShape()
{
}

cPyramidShape::~cPyramidShape()
{
}


bool cPyramidShape::Create(cRenderer &renderer
	, const float width //=1
	, const float height//=1
	, const Vector3 &pos //=Vector3(0,0,0)
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= cColor::BLACK
)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return true; // already created

//
//            0
//           /  \
//          /    \
//       1,2 ----- 3,4
//
//       1 ----------- 3
//       | \           |
//       |    \        |
//       |      +      |
//       |       \     |
//       |          \  |
//       2 ----------- 4
//
//				Z
//				|
//				|
//				|
//        ----------------> X
//

	Vector3 vertices[] = {
		Vector3(0,1,0)
		, Vector3(-0.5f,0,0.5f)
		, Vector3(-0.5f,0,-0.5f)
		, Vector3(0.5f,0,0.5f)
		, Vector3(0.5f,0,-0.5f)
	};

	const Vector3 xNorm = (Vector3(0, 1, 0) + Vector3(0.5f, 0, 0)).Normal();
	const Vector3 xNorm2 = (Vector3(0, 1, 0) + Vector3(-0.5f, 0, 0)).Normal();
	const Vector3 zNorm = (Vector3(0, 1, 0) + Vector3(0, 0, 0.5f)).Normal();
	const Vector3 zNorm2 = (Vector3(0, 1, 0) + Vector3(0, 0, -0.5f)).Normal();

	Vector3 normals[] = {
		// bottom
		Vector3(0,-1,0), Vector3(0,-1,0), Vector3(0,-1,0)
		, Vector3(0,-1,0), Vector3(0,-1,0), Vector3(0,-1,0)
		, zNorm, zNorm, zNorm
		, xNorm, xNorm, xNorm
		, zNorm2, zNorm2, zNorm2
		, xNorm2, xNorm2, xNorm2
	};

	int indices[] = {
		// bottom
		1,2,4
		,1,4,3
		,0,1,3 // Z axis
		,0,3,4 // X axis
		,0,4,2 // -Z axis
		,0,2,1 // -X axis
	};

	cVertexLayout vtxLayout;
	vtxLayout.Create(vtxType);
	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();

	vector<BYTE> buffer(vertexStride * ARRAYSIZE(indices));
	BYTE *initVertices = &buffer[0];
	const Vector4 vColor = color.GetColor();
	BYTE *pvtx = initVertices;
	for (uint i=0; i < ARRAYSIZE(indices); ++i)
	{
		const Vector3 &v = vertices[indices[i]];
		const Vector3 &n = normals[i];

		if (vtxType & eVertexType::POSITION)
			*(Vector3*)(pvtx + posOffset) = v;
		if (vtxType & eVertexType::NORMAL)
			*(Vector3*)(pvtx + normOffset) = n;
		if (vtxType & eVertexType::COLOR)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		//if (vtxType & eVertexType::TEXTURE0)
		//	*(Vector2*)(pvtx + texOffset) = uv[i];
		pvtx += vertexStride;
	}

	vector<WORD> buffer2(ARRAYSIZE(indices));
	WORD *initIndices = &buffer2[0];
	for (int i = 0; i < ARRAYSIZE(indices); ++i)
		*initIndices++ = indices[i];

	m_vtxBuff.Create(renderer, ARRAYSIZE(indices), vertexStride, initVertices);

	m_vtxType = vtxType;

	return true;
}


void cPyramidShape::Render(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->Draw(m_vtxBuff.GetVertexCount(), 0);
}
