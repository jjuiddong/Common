
#include "stdafx.h"
#include "meshbuffer.h"


using namespace graphic;

cMeshBuffer::cMeshBuffer()
:	m_offset(0)
, m_isSkinned(false)
{
}

cMeshBuffer::cMeshBuffer(cRenderer &renderer, const sRawMesh2 &rawMesh)
	: m_offset(0)
	, m_isSkinned(false)
{
	CreateMesh(renderer, rawMesh);
	//CreateAttributes(rawMesh);
}

cMeshBuffer::~cMeshBuffer()
{
	Clear();
}


void cMeshBuffer::CreateMesh(cRenderer &renderer, const sRawMesh2 &rawMesh)
{
	cVertexLayout layout;
	layout.Create(rawMesh);

	CreateMesh(renderer,
		layout,
		rawMesh.vertices,
		rawMesh.normals,
		rawMesh.tex,
		rawMesh.tangent,
		rawMesh.binormal,
		rawMesh.weights,
		rawMesh.indices
	);
}


// Create Mesh
void cMeshBuffer::CreateMesh(cRenderer &renderer,
	const cVertexLayout &layout,
	const vector<Vector3> &vertices,
	const vector<Vector3> &normals,
	const vector<Vector3> &tex,
	const vector<Vector3> &tangent,
	const vector<Vector3> &binormal,
	const vector<sVertexWeight> &weights,
	const vector<int> &indices)
{
	const bool isTexture = !tex.empty();

	const int vertexStride = layout.GetVertexSize();
	vector<BYTE> buffer(vertexStride * vertices.size());
	BYTE *pv = &buffer[0];

	sMinMax minMax;
	const int pos_offset = layout.GetOffset("POSITION");
	for (u_int i = 0; i < vertices.size(); i++)
	{
		BYTE *p = pv + (vertexStride * i);
		Vector3 *position = (Vector3*)(p + pos_offset);
		*position = vertices[i];
		minMax.Update(vertices[i]);
	}
	m_boundingBox.SetBoundingBox(minMax);

	// normal
	const int norm_offset = layout.GetOffset("NORMAL");
	for (u_int i = 0; i < normals.size(); i++)
	{
		BYTE *p = pv + (vertexStride * i);
		Vector3 *pn = (Vector3*)(p + norm_offset);
		*pn = normals[i];
	}

	// texture
	const int tex_offset = layout.GetOffset("TEXCOORD");
	for (u_int i = 0; i < tex.size(); i++)
	{
		BYTE *p = pv + (vertexStride * i);
		Vector2 *pt = (Vector2*)(p + tex_offset);
		pt->x = tex[i].x;
		pt->y = tex[i].y;
	}

	// tagent
	//const int tangent_offset = decl.GetOffset(D3DDECLUSAGE_TANGENT);
	//for (u_int i = 0; i < tangent.size(); i++)
	//{
	//	BYTE *p = pv + (decl.GetElementSize() * i);
	//	Vector3 *pt = (Vector3*)(p + tangent_offset);
	//	*pt = tangent[i];
	//}

	//// binormal
	//const int binormal_offset = decl.GetOffset(D3DDECLUSAGE_BINORMAL);
	//for (u_int i = 0; i < binormal.size(); i++)
	//{
	//	BYTE *p = pv + (decl.GetElementSize() * i);
	//	Vector3 *pb = (Vector3*)(p + binormal_offset);
	//	*pb = binormal[i];
	//}

	//// bone weight
	//m_isSkinned = !weights.empty();
	//const int blendWeight_offset = decl.GetOffset(D3DDECLUSAGE_TEXCOORD, 1);
	//const int blendIndices_offset = decl.GetOffset(D3DDECLUSAGE_TEXCOORD, 2);
	//for (u_int i = 0; i < weights.size(); i++)
	//{
	//	BYTE *p = pv + (decl.GetElementSize() * i);
	//	float *vtxWeight = (float*)(p + blendWeight_offset); // float4
	//	float *vtxIndices = (float*)(p + blendIndices_offset); // float4

	//	const sVertexWeight &weight = weights[i];
	//	const int vtxIdx = weight.vtxIdx;

	//	ZeroMemory(vtxWeight, sizeof(float) * 4);
	//	ZeroMemory(vtxIndices, sizeof(float) * 4);
	//	//pv[ vtxIdx].matrixIndices = 0;

	//	for (int k = 0; (k < weight.size) && (k < 4); ++k)
	//	{
	//		const sWeight *w = &weight.w[k];
	//		if (k < 3)
	//		{
	//			vtxWeight[k] = w->weight;
	//		}
	//		else // k == 3 (마지막 가중치)
	//		{
	//			vtxWeight[k] =
	//				1.f - (vtxWeight[0] + vtxWeight[1] + vtxWeight[2]);
	//		}

	//		vtxIndices[k] = (float)w->bone;
	//		//const int boneIdx = (w->bone << (8*(3-k)));
	//		//pv[ vtxIdx].matrixIndices |= boneIdx;
	//	}
	//}

	m_vtxBuff.Create(renderer, vertices.size(), vertexStride, &buffer[0]);


	// 인덱스 버퍼 생성.
	vector<WORD> buffer2(indices.size());
	WORD *pi = &buffer2[0];
	for (u_int i = 0; i < indices.size(); ++i)
		pi[i] = indices[i];
	m_idxBuff.Create(renderer, indices.size()/3, &buffer2[0]);
}


// 속성버퍼 초기화.
void cMeshBuffer::CreateAttributes(const sRawMesh &rawMesh)
{
	m_attributes = rawMesh.attributes;
}


// 인스턴스를 깊이복사해서 리턴한다.
cMeshBuffer* cMeshBuffer::Clone()
{
	cMeshBuffer *mesh = new cMeshBuffer();
	mesh->m_attributes = m_attributes;
	mesh->m_vtxBuff = m_vtxBuff;
	mesh->m_idxBuff = m_idxBuff;
	return mesh;
}


// 출력. 
void cMeshBuffer::Render(cRenderer &renderer
	, const int faceStart	// =0
	, const int faceCount	// =0
)
{
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed((faceCount==0)? (m_idxBuff.GetFaceCount() * 3) : (faceCount * 3), faceStart, m_offset);
}


void cMeshBuffer::RenderInstancing(cRenderer &renderer
	, const int count
	, const int faceStart //= 0
	, const int faceCount //= 0
)
{
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	renderer.GetDevContext()->DrawIndexedInstanced(
		(faceCount == 0) ? (m_idxBuff.GetFaceCount() * 3) : (faceCount * 3)
		, count
		, faceStart
		, m_offset
		, 0);
}


bool cMeshBuffer::IsLoaded()
{
	return m_vtxBuff.GetVertexCount() > 0;
}


void cMeshBuffer::Clear()
{
	m_attributes.clear();
	m_vtxBuff.Clear();
	m_idxBuff.Clear();
}
