
#include "stdafx.h"
#include "cubeshape.h"


namespace {

}

using namespace graphic;


cCubeShape::cCubeShape()
{
}

cCubeShape::~cCubeShape()
{
}


// Texture Cube
bool cCubeShape::Create1(
	cRenderer &renderer
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= Color::BLACK
)
{
	return InitCube1(renderer, vtxType, color);
}


// Vertex Cube
bool cCubeShape::Create2(cRenderer &renderer
	, const int vtxType // = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= cColor::BLACK
)
{
	return InitCube2(renderer, vtxType, color);
}


bool cCubeShape::InitCube1(cRenderer &renderer
	, const int vtxType // = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= Color::BLACK
)
{
	return CreateShape1(renderer, g_cubeVertices1, g_cubeNormals1, g_cubeUV1, g_cubeIndices1, vtxType, color);
}


bool cCubeShape::InitCube2(cRenderer &renderer
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= Color::BLACK
)
{
	return CreateShape2(renderer, g_cubeVertices2, g_cubeIndices2, g_cubeUV2, vtxType, color);
}


bool cCubeShape::CreateShape2(cRenderer &renderer
	, const Vector3 vertices[8], const WORD indices[36], const Vector2 uvs[36]
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR);
	, const cColor &color //= cColor::BLACK
)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return false;

	cVertexLayout vtxLayout;
	vtxLayout.Create(vtxType);
	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();

	vector<BYTE> buffer(vertexStride * 36);
	BYTE *initVertices = &buffer[0];
	const Vector4 vColor = color.GetColor();
	BYTE *pvtx = initVertices;
	for (int i = 0; i < 12; ++i)
	{
		Vector3 v1 = vertices[indices[i * 3]];
		Vector3 v2 = vertices[indices[i * 3 + 1]];
		Vector3 v3 = vertices[indices[i * 3 + 2]];
		Vector2 uv1 = uvs[i * 3];
		Vector2 uv2 = uvs[i * 3 + 1];
		Vector2 uv3 = uvs[i * 3 + 2];
		Vector3 normal = (v2 - v1).Normal().CrossProduct((v3 - v1).Normal());

		if ((vtxType & eVertexType::POSITION) || (vtxType & eVertexType::POSITION_RHW))
			*(Vector3*)(pvtx + posOffset) = v1;
		if (vtxType & eVertexType::NORMAL)
			*(Vector3*)(pvtx + normOffset) = normal;
		if (vtxType & eVertexType::COLOR)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		if (vtxType & eVertexType::TEXTURE)
			*(Vector2*)(pvtx + texOffset) = uv1;
		pvtx += vertexStride;

		if ((vtxType & eVertexType::POSITION) || (vtxType & eVertexType::POSITION_RHW))
			*(Vector3*)(pvtx + posOffset) = v2;
		if (vtxType & eVertexType::NORMAL)
			*(Vector3*)(pvtx + normOffset) = normal;
		if (vtxType & eVertexType::COLOR)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		if (vtxType & eVertexType::TEXTURE)
			*(Vector2*)(pvtx + texOffset) = uv2;
		pvtx += vertexStride;

		if ((vtxType & eVertexType::POSITION) || (vtxType & eVertexType::POSITION_RHW))
			*(Vector3*)(pvtx + posOffset) = v3;
		if (vtxType & eVertexType::NORMAL)
			*(Vector3*)(pvtx + normOffset) = normal;
		if (vtxType & eVertexType::COLOR)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		if (vtxType & eVertexType::TEXTURE)
			*(Vector2*)(pvtx + texOffset) = uv3;
		pvtx += vertexStride;
	}

	m_vtxBuff.Create(renderer, 36, vertexStride, initVertices, D3D11_USAGE_DYNAMIC);

	vector<WORD> buffer2(36);
	WORD *initIndices = &buffer2[0];
	for (int i = 0; i < 36; ++i)
		initIndices[i] = i;

	m_idxBuff.Create(renderer, 12, (BYTE*)initIndices);

	m_vtxType = vtxType;

	return true;
}


bool cCubeShape::CreateShape1(cRenderer &renderer, const Vector3 vertices[24]
	, const Vector3 normals[24], const Vector2 uvs[24], const WORD indices[36]
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= cColor::BLACK
)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return false;

	cVertexLayout vtxLayout;
	vtxLayout.Create(vtxType);
	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();
	vector<BYTE> buffer(vertexStride * 24);
	BYTE *initVertices = &buffer[0];

	const Vector4 vColor = color.GetColor();
	BYTE *pvtx = initVertices;
	for (int i = 0; i < 24; ++i)
	{
		if ((vtxType & eVertexType::POSITION) || (vtxType & eVertexType::POSITION_RHW))
			*(Vector3*)(pvtx + posOffset) = vertices[i];
			*(Vector3*)(pvtx + posOffset) = vertices[i];
		if (vtxType & eVertexType::NORMAL)
			*(Vector3*)(pvtx + normOffset) = normals[i];
		if (vtxType & eVertexType::COLOR)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		if (vtxType & eVertexType::TEXTURE)
			*(Vector2*)(pvtx + texOffset) = uvs[i];
		pvtx += vertexStride;
	}

	m_vtxBuff.Create(renderer, 24, vertexStride, initVertices);

	vector<WORD> buffer2(36);
	WORD *initIndices = &buffer2[0];
	for (int i = 0; i < 36; ++i)
		initIndices[i] = indices[i];
	m_idxBuff.Create(renderer, 12, (BYTE*)initIndices);

	m_vtxType = vtxType;

	return true;
}


void cCubeShape::Render(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 3, 0, 0);
}


void cCubeShape::SetCube(cRenderer &renderer, const Vector3 vertices[8]
	, const cColor &color //= cColor::BLACK
)
{
	SetShape2(renderer, vertices, g_cubeIndices2, color);
}


// Update Shape Vertex
void cCubeShape::SetShape2(cRenderer &renderer, const Vector3 vertices[8]
	, const WORD indices[36]
	, const cColor &color // = cColor::BLACK
)
{
	cVertexLayout vtxLayout;
	vtxLayout.Create(m_vtxType);
	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();

	const Vector4 vColor = color.GetColor();
	if (BYTE *initVertices = (BYTE*)m_vtxBuff.Lock(renderer))
	{
		BYTE *pvtx = initVertices;
		for (int i = 0; i < 12; ++i)
		{
			Vector3 v1 = vertices[indices[i * 3]];
			Vector3 v2 = vertices[indices[i * 3 + 1]];
			Vector3 v3 = vertices[indices[i * 3 + 2]];
			Vector3 normal = (v2 - v1).Normal().CrossProduct((v3 - v1).Normal());

			if (m_vtxType & eVertexType::POSITION)
				*(Vector3*)(pvtx + posOffset) = v1;
			if (m_vtxType & eVertexType::NORMAL)
				*(Vector3*)(pvtx + normOffset) = normal;
			if (m_vtxType & eVertexType::COLOR)
				*(Vector4*)(pvtx + colorOffset) = vColor;
			pvtx += vertexStride;

			if (m_vtxType & eVertexType::POSITION)
				*(Vector3*)(pvtx + posOffset) = v2;
			if (m_vtxType & eVertexType::NORMAL)
				*(Vector3*)(pvtx + normOffset) = normal;
			if (m_vtxType & eVertexType::COLOR)
				*(Vector4*)(pvtx + colorOffset) = vColor;
			pvtx += vertexStride;

			if (m_vtxType & eVertexType::POSITION)
				*(Vector3*)(pvtx + posOffset) = v3;
			if (m_vtxType & eVertexType::NORMAL)
				*(Vector3*)(pvtx + normOffset) = normal;
			if (m_vtxType & eVertexType::COLOR)
				*(Vector4*)(pvtx + colorOffset) = vColor;
			pvtx += vertexStride;
		}

		m_vtxBuff.Unlock(renderer);
	}
}
