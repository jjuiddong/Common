
#include "stdafx.h"
#include "cube.h"

using namespace graphic;


cCube::cCube()
	: m_cubeType((eCubeType::POSITION | eCubeType::NORMAL | eCubeType::DIFFUSE))
{
}

cCube::cCube(cRenderer &renderer, const cBoundingBox &bbox
	, const int cubeType //= (eCubeType::POSITION | eCubeType::NORMAL | eCubeType::DIFFUSE)
	, const cColor &color //= cColor::WHITE
)
{
	InitCube(renderer, cubeType, color);
	SetCube(renderer, bbox);
}


bool cCube::Create(cRenderer &renderer, const cBoundingBox &bbox
	, const int cubeType //= (eCubeType::POSITION | eCubeType::NORMAL | eCubeType::DIFFUSE)
	, const cColor &color //= Color::WHITE
)
{
	InitCube(renderer, cubeType, color);
	SetCube(bbox);

	return true;
}


void cCube::InitCube(cRenderer &renderer, const int cubeType
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
	if (cubeType & eCubeType::POSITION)
		elems.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (cubeType & eCubeType::NORMAL)
		elems.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (cubeType & eCubeType::DIFFUSE)
		elems.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (cubeType & eCubeType::TEXTURE)
		elems.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	cVertexLayout vtxLayout;
	vtxLayout.Create(elems);

	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();
	BYTE *initVertices = new BYTE[vertexStride * 24];

	const Vector4 vColor = color.GetColor();
	BYTE *pvtx = initVertices;
	for (int i = 0; i < 24; ++i)
	{
		if (cubeType & eCubeType::POSITION)
			*(Vector3*)(pvtx + posOffset) = vertices[i];
		if (cubeType & eCubeType::NORMAL)
			*(Vector3*)(pvtx + normOffset) = normals[i];
		if (cubeType & eCubeType::DIFFUSE)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		if (cubeType & eCubeType::TEXTURE)
			*(Vector2*)(pvtx + texOffset) = uv[i];
		pvtx += vertexStride;
	}

	m_vtxBuff.Create(renderer, 24, vertexStride, initVertices);
	delete[] initVertices;


	WORD *initIndices = new WORD[36 * sizeof(WORD)];
	for (int i = 0; i < 36; ++i)
		initIndices[i] = indices[i];
	m_idxBuff.Create(renderer, 12, initIndices);
	delete[] initIndices;

	m_boundingBox.SetBoundingBox(Vector3(-1, -1, -1), Vector3(1, 1, 1));
	m_cubeType = cubeType;
}


void cCube::SetCube(const cBoundingBox &bbox)
{
	const Vector3 vMin = bbox.m_min;
	const Vector3 vMax = bbox.m_max;
	const Vector3 center = (vMin + vMax) / 2.f;
	const Vector3 v1 = vMin - vMax;
	Vector3 scale(abs(v1.x) / 2, abs(v1.y) / 2, abs(v1.z) / 2);

	m_transform.scale = scale;
	m_transform.pos = center;
	m_boundingBox = bbox;
}


void cCube::SetCube(cRenderer &renderer, const cBoundingBox &bbox)
{
	if (m_vtxBuff.GetVertexCount() <= 0)
		InitCube(renderer, m_cubeType);
	SetCube(bbox);
}


void cCube::SetCube(cRenderer &renderer, const cCube &cube)
{
	SetCube(renderer, cube.m_boundingBox);
}


void cCube::SetColor(const cColor &color)
{
	//sVertexNormDiffuse *vbuff = (sVertexNormDiffuse*)m_vtxBuff.Lock();
	//for (int i=0; i < m_vtxBuff.GetVertexCount(); ++i)
	//	vbuff[ i].c = color;
	//m_vtxBuff.Unlock();
}


bool cCube::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);

	if ((m_cubeType & eCubeType::TEXTURE) && m_texture)
		m_texture->Bind(renderer, 0);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 3, 0, 0);

	__super::Render(renderer, tm, flags);
	return true;
}

