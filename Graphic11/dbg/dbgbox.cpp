
#include "stdafx.h"
#include "dbgbox.h"

using namespace graphic;


cDbgBox::cDbgBox()
{
}

cDbgBox::cDbgBox(cRenderer &renderer, const cBoundingBox &bbox
	, const cColor &color //= cColor::BLACK
)
{
	Create(renderer, bbox, color);
}


void cDbgBox::Create(cRenderer &renderer
	, const cColor &color //= cColor::BLACK
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
	for (int i = 0; i < 24; ++i)
		vertices[i] *= 0.5f;

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
	elems.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	elems.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	cVertexLayout vtxLayout;
	vtxLayout.Create(elems);

	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int vertexStride = vtxLayout.GetVertexSize();

	vector<BYTE> buffer(vertexStride * 24);
	BYTE *initVertices = &buffer[0];
	const Vector4 vColor = color.GetColor();
	BYTE *pvtx = initVertices;
	for (int i = 0; i < 24; ++i)
	{
		*(Vector3*)(pvtx + posOffset) = vertices[i];
		*(Vector4*)(pvtx + colorOffset) = vColor;
		pvtx += vertexStride;
	}

	m_vtxBuff.Create(renderer, 24, vertexStride, initVertices);

	vector<WORD> buffer2(36);
	WORD *initIndices = &buffer2[0];
	for (int i = 0; i < 36; ++i)
		initIndices[i] = indices[i];

	m_idxBuff.Create(renderer, 12, initIndices);

	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0,0,0,1));
}


void cDbgBox::Create(cRenderer &renderer, const cBoundingBox &bbox
	, const cColor &color //= cColor::BLACK
)
{
	if (m_vtxBuff.GetVertexCount() <= 0)
		Create(renderer, color);

	SetBox(bbox);
}


void cDbgBox::SetBox(const cBoundingBox &bbox)
{
	m_boundingBox = bbox;
}


void cDbgBox::SetColor(DWORD color)
{
	//sVertexDiffuse *vbuff = (sVertexDiffuse*)m_vtxBuff.Lock();
	//for (int i = 0; i < m_vtxBuff.GetVertexCount(); ++i)
	//	vbuff[i].c = color;
	//m_vtxBuff.Unlock();
}


// Render Box using Triangle
void cDbgBox::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	RET(m_vtxBuff.GetVertexCount() <= 0);

	XMMATRIX ctm = m_boundingBox.GetTransform();

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(states.Wireframe());

	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 3, 0, 0);

	renderer.GetDevContext()->RSSetState(NULL);

	//DWORD cullMode;
	//DWORD fillMode;
	//DWORD lightMode;
	//renderer.GetDevice()->GetRenderState(D3DRS_CULLMODE, &cullMode);
	//renderer.GetDevice()->GetRenderState(D3DRS_FILLMODE, &fillMode);
	//renderer.GetDevice()->GetRenderState(D3DRS_LIGHTING, &lightMode);

	//renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, FALSE);
	//renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	//renderer.GetDevice()->SetTexture(0, NULL);

	//Matrix44 mat = m_tm * tm;
	//renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&mat);
	//m_vtxBuff.Bind(renderer);
	//m_idxBuff.Bind(renderer);
	//renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
	//	m_vtxBuff.GetVertexCount(), 0, 12);

	//renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, cullMode);
	//renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, fillMode);
	//renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lightMode);
}
