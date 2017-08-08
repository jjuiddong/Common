
#include "stdafx.h"
#include "quadshape.h"

using namespace graphic;


cQuadShape::cQuadShape()
{
}

cQuadShape::cQuadShape(cRenderer &renderer
	, const int vtxType //= (eVertexType::POSITION | eVertexType::TEXTURE)
	, const cColor &color //= cColor::WHITE
)
{
	Create(renderer, vtxType, color);
}


cQuadShape::~cQuadShape()
{
}


bool cQuadShape::Create(cRenderer &renderer
	, const int vtxType //= (eVertexType::POSITION | eVertexType::TEXTURE)
	, const cColor &color //= cColor::WHITE
)
{
	const float width = 2;
	const float height = 2;

	Vector3 vertices[4] = {
		Vector3(-width / 2.f, 0, height / 2.f)
		,Vector3(width / 2.f, 0, height / 2.f)
		,Vector3(-width / 2.f, 0, -height / 2.f)
		,Vector3(width / 2.f, 0, -height / 2.f)
	};

	Vector2 uv[4] = {
		Vector2(0,0)
		, Vector2(1,0)
		, Vector2(0,1)
		, Vector2(1,1)
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
	for (int i = 0; i < 4; ++i)
	{
		if (vtxType & eVertexType::POSITION)
			*(Vector3*)(pvtx + posOffset) = vertices[i];
		//if (vtxType & eVertexType::NORMAL)
		//	*(Vector3*)(pvtx + normOffset) = normals[i];
		//if (vtxType & eVertexType::DIFFUSE)
		//	*(Vector4*)(pvtx + colorOffset) = vColor;
		if (vtxType & eVertexType::TEXTURE)
			*(Vector2*)(pvtx + texOffset) = uv[i];
		pvtx += vertexStride;
	}

	m_vtxBuff.Create(renderer, 4, vertexStride, initVertices);

	return true;
}


void cQuadShape::Render(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	renderer.GetDevContext()->DrawInstanced(4, 1, 0, 0);
}
