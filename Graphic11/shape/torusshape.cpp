
#include "stdafx.h"
#include "torusshape.h"

using namespace graphic;


cTorusShape::cTorusShape()
	: cShape()
{
}

cTorusShape::~cTorusShape()
{
	Clear();
}


bool cTorusShape::Create(cRenderer &renderer, const float outerRadius, const float innerRadius
	, const int stack //= 10
	, const int slice //=10
	, const int vtxType //= (eVertexType::POSITION)
)
{
	RETV2(stack <= 0, false);
	RETV2(slice <= 0, false);

	if (m_vtxBuff.GetVertexCount() > 0)
		return false;

	//                     Z
	//                     |
	//                  4 *|* 3
	//                5*   |   * 2
	//              6*     |      * 1
	//             7*      |       * 0 (vertex order)
	//            8*------------------> X
	//             9*      |       *16
	//             10*     |      *15   
	//               11*   |    *14         
	//                  12*|*13          
	//
	// Counter Clock Order
	
	const float radius = (outerRadius + innerRadius) / 2.f;
	const float circleR = (outerRadius - innerRadius) / 2;
	const float incAngle = (MATH_PI * 2.f) / (float)stack;

	vector<Vector3> vertices;
	vertices.reserve(stack*slice);

	float angle = 0; // radian
	for (int i = 0; i < stack; ++i)
	{
		Vector3 center(cos(angle) * radius, 0, sin(angle) * radius);

		const float incCircleAngle = (MATH_PI * 2.f) / (float)slice;
		float circleAnlge = 0;

		for (int k = 0; k < slice; ++k)
		{
			const float y = sin(circleAnlge) * circleR;
			const float cosV = cos(circleAnlge) * circleR;
			const Vector3 pos(cos(angle) * cosV, y, sin(angle) * cosV);
			vertices.push_back(pos + center);
			circleAnlge += incCircleAngle;
		}
		angle += incAngle;
	}

	vector<WORD> indices;
	indices.reserve(slice * stack * 2 * 3);
	for (int i = 0; i < stack; ++i)
	{
		for (int k = 0; k < slice; ++k)
		{
			// triangle1
			int nextI = (i + 1) % stack;
			int nextK = (k + 1) % slice;
			indices.push_back(slice*i + k); // p1
			indices.push_back(slice*i + nextK); // p2
			indices.push_back(slice*(nextI) + k); // p2

			// triangle2
			indices.push_back(slice*i + nextK); // p1
			indices.push_back(slice*nextI + nextK); // p2
			indices.push_back(slice*nextI + k); // p2
		}
	}

	// Create VertexBuffer
	vector<D3D11_INPUT_ELEMENT_DESC> elems;
	if (vtxType & eVertexType::POSITION)
		elems.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vtxType & eVertexType::NORMAL)
		elems.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	if (vtxType & eVertexType::COLOR)
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

	vector<BYTE> buffer(vertexStride * vertices.size());
	BYTE *initVertices = &buffer[0];
	const Vector4 vColor = cColor::WHITE.GetColor();// color.GetColor();
	BYTE *pvtx = initVertices;
	for (u_int i = 0; i < vertices.size(); ++i)
	{
		Vector3 v1 = vertices[i];

		if (vtxType & eVertexType::POSITION)
			*(Vector3*)(pvtx + posOffset) = v1;
		//if (vtxType & eVertexType::NORMAL)
		//	*(Vector3*)(pvtx + normOffset) = normal;
		if (vtxType & eVertexType::COLOR)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		pvtx += vertexStride;
	}

	m_vtxBuff.Create(renderer, vertices.size(), vertexStride, initVertices, D3D11_USAGE_DEFAULT);

	// Create Index Buffer
	m_idxBuff.Create(renderer, indices.size() / 3, (BYTE*)&indices[0]);

	m_vtxType = vtxType;

	return true;
}


void cTorusShape::Render(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 3, 0, 0);
}


void cTorusShape::Clear()
{
	m_vtxBuff.Clear();
	m_idxBuff.Clear();
}
