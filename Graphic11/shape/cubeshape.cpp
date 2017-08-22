
#include "stdafx.h"
#include "cubeshape.h"


using namespace graphic;


cCubeShape::cCubeShape()
{
}

cCubeShape::cCubeShape(
	cRenderer &renderer
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
	, const cColor &color //= cColor::WHITE
)
{
	InitCube(renderer, vtxType, color);
}


cCubeShape::~cCubeShape()
{
}


bool cCubeShape::Create(
	cRenderer &renderer
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
	, const cColor &color //= Color::WHITE
)
{
	InitCube(renderer, vtxType, color);
	return true;
}


void cCubeShape::InitCube(cRenderer &renderer, const int vtxType
	, const cColor &color //= Color::WHITE
)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return;

	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	Vector3 vertices[24] = {
		Vector3(-1.f, 1.f, -1.f),//0
		Vector3(1.f, -1.f, -1.f),//3
		Vector3(-1.f, -1.f, -1.f),//2
		Vector3(1.f, 1.f, -1.f),//1
		Vector3(-1.f, -1.f, -1.f),//2
		Vector3(1.f, -1.f, 1.f),// 7
		Vector3(-1.f, -1.f, 1.f),//6
		Vector3(1.f, -1.f, -1.f),//3
		Vector3(-1.f, -1.f, 1.f),
		Vector3(1.f, 1.f, 1.f),
		Vector3(-1.f, 1.f, 1.f),
		Vector3(1.f, -1.f, 1.f),
		Vector3(-1.f, 1.f, 1.f),
		Vector3(1.f, 1.f, -1.f),
		Vector3(-1.f, 1.f, -1.f),
		Vector3(1.f, 1.f, 1.f),
		Vector3(-1.f, -1.f, 1.f),
		Vector3(-1.f, 1.f, -1.f),
		Vector3(-1.f, -1.f, -1.f),
		Vector3(-1.f, 1.f, 1.f),
		Vector3(1.f, -1.f, 1.f),
		Vector3(1.f, 1.f, -1.f),
		Vector3(1.f, 1.f, 1.f),
		Vector3(1.f, -1.f, -1.f),
	};

	Vector3 normals[24] = {
		Vector3(0.f, 0.f, -1.f),
		Vector3(0.f, 0.f, -1.f),
		Vector3(0.f, 0.f, -1.f),
		Vector3(0.f, 0.f, -1.f),
		Vector3(0.f, -1.f, 0.f),
		Vector3(0.f, -1.f, 0.f),
		Vector3(0.f, -1.f, 0.f),
		Vector3(0.f, -1.f, 0.f),
		Vector3(0.f, 0.f, 1.f),
		Vector3(0.f, 0.f, 1.f),
		Vector3(0.f, 0.f, 1.f),
		Vector3(0.f, 0.f, 1.f),
		Vector3(0.f, 1.f, 0.f),
		Vector3(0.f, 1.f, 0.f),
		Vector3(0.f, 1.f, 0.f),
		Vector3(0.f, 1.f, 0.f),
		Vector3(-1.f, 0.f, 0.f),
		Vector3(-1.f, 0.f, 0.f),
		Vector3(-1.f, 0.f, 0.f),
		Vector3(-1.f, 0.f, 0.f),
		Vector3(1.f, 0.f, 0.f),
		Vector3(1.f, 0.f, 0.f),
		Vector3(1.f, 0.f, 0.f),
		Vector3(1.f, 0.f, 0.f),
	};
	Vector2 uv[24] = {
		Vector2(0.278796f, 0.399099f),
		Vector2(0.008789f, 0.598055f),
		Vector2(0.008789f, 0.399099f),
		Vector2(0.278796f, 0.598055f),
		Vector2(0.987252f, 0.399099f),
		Vector2(0.768028f, 0.598055f),
		Vector2(0.768028f, 0.399099f),
		Vector2(0.987252f, 0.598055f),
		Vector2(0.768028f, 0.399099f),
		Vector2(0.49802f, 0.598055f),
		Vector2(0.49802f, 0.399099f),
		Vector2(0.768028f, 0.598055f),
		Vector2(0.49802f, 0.399099f),
		Vector2(0.278796f, 0.598055f),
		Vector2(0.278796f, 0.399099f),
		Vector2(0.49802f, 0.598055f),
		Vector2(0.49802f, 0.198324f),
		Vector2(0.278796f, 0.399099f),
		Vector2(0.278796f, 0.198324f),
		Vector2(0.49802f, 0.399099f),
		Vector2(0.49802f, 0.79883f),
		Vector2(0.278796f, 0.598055f),
		Vector2(0.49802f, 0.598055f),
		Vector2(0.278796f, 0.79883f),
	};
	WORD indices[36] = {
		0, 1, 2,
		1, 0, 3,
		4, 5, 6,
		5, 4, 7,
		8, 9, 10,
		9, 8, 11,
		12, 13, 14,
		13, 12, 15,
		16, 17, 18,
		17, 16, 19,
		20, 21, 22,
		21, 20, 23,
	};

	vector<D3D11_INPUT_ELEMENT_DESC> elems;
	if (vtxType & eVertexType::POSITION)
		elems.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vtxType & eVertexType::NORMAL)
		elems.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vtxType & eVertexType::DIFFUSE)
		elems.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vtxType & eVertexType::TEXTURE)
		elems.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	cVertexLayout vtxLayout;
	vtxLayout.Create(elems);

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
		if (vtxType & eVertexType::POSITION)
			*(Vector3*)(pvtx + posOffset) = vertices[i];
		if (vtxType & eVertexType::NORMAL)
			*(Vector3*)(pvtx + normOffset) = normals[i];
		if (vtxType & eVertexType::DIFFUSE)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		if (vtxType & eVertexType::TEXTURE)
			*(Vector2*)(pvtx + texOffset) = uv[i];
		pvtx += vertexStride;
	}

	m_vtxBuff.Create(renderer, 24, vertexStride, initVertices);

	vector<WORD> buffer2(36);
	WORD *initIndices = &buffer2[0];
	for (int i = 0; i < 36; ++i)
		initIndices[i] = indices[i];
	m_idxBuff.Create(renderer, 12, initIndices);

	m_vtxType = vtxType;
}


void cCubeShape::Create(cRenderer &renderer, Vector3 vertices[8]
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
	, const cColor &color //= cColor::BLACK
)
{
	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	// vertices

	WORD indices[36] = {
		// front
		0, 3, 2,
		0 ,1, 3,
		// back
		5, 6, 7,
		5, 4, 6,
		// top
		4, 1, 0,
		4, 5, 1,
		// bottom
		2, 7, 6,
		2, 3, 7,
		// left
		4, 2, 6,
		4, 0, 2,
		// right
		1, 7, 3,
		1, 5, 7,
	};

	vector<D3D11_INPUT_ELEMENT_DESC> elems;
	if (vtxType & eVertexType::POSITION)
		elems.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vtxType & eVertexType::NORMAL)
		elems.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vtxType & eVertexType::DIFFUSE)
		elems.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vtxType & eVertexType::TEXTURE)
		elems.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	cVertexLayout vtxLayout;
	vtxLayout.Create(elems);

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
		Vector3 normal = (v2 - v1).Normal().CrossProduct((v3 - v1).Normal());


		if (vtxType & eVertexType::POSITION)
			*(Vector3*)(pvtx + posOffset) = v1;
		if (vtxType & eVertexType::NORMAL)
			*(Vector3*)(pvtx + normOffset) = normal;
		if (vtxType & eVertexType::DIFFUSE)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		pvtx += vertexStride;

		if (vtxType & eVertexType::POSITION)
			*(Vector3*)(pvtx + posOffset) = v2;
		if (vtxType & eVertexType::NORMAL)
			*(Vector3*)(pvtx + normOffset) = normal;
		if (vtxType & eVertexType::DIFFUSE)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		pvtx += vertexStride;

		if (vtxType & eVertexType::POSITION)
			*(Vector3*)(pvtx + posOffset) = v3;
		if (vtxType & eVertexType::NORMAL)
			*(Vector3*)(pvtx + normOffset) = normal;
		if (vtxType & eVertexType::DIFFUSE)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		pvtx += vertexStride;
	}

	m_vtxBuff.Create(renderer, 36, vertexStride, initVertices);

	vector<WORD> buffer2(36);
	WORD *initIndices = &buffer2[0];
	for (int i = 0; i < 36; ++i)
		initIndices[i] = i;

	m_idxBuff.Create(renderer, 12, initIndices);

	m_vtxType = vtxType;
}


void cCubeShape::Render(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 3, 0, 0);
}

