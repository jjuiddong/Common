
#include "stdafx.h"
#include "capsuleshape.h"

using namespace graphic;


cCapsuleShape::cCapsuleShape()
{
}

cCapsuleShape::~cCapsuleShape()
{
}


// create capsule shape
// height: half height
bool cCapsuleShape::Create(cRenderer &renderer
	, const float radius
	, const float height
	, const int stacks
	, const int slices
	, const int vtxType // = (eVertexType::POSITION | eVertexType::NORMAL| eVertexType::COLOR)
	, const cColor &color // = cColor::BLACK
)
{
	if (m_vtxBuff1.GetVertexCount() > 0)
		return false; // already exist

	m_stacks = stacks;
	m_slices = slices;

	// side face (cylinder)
	{
		const int faceCount = slices * 2;
		const int vtxCount = faceCount * 3;
		cVertexLayout vtxLayout;
		vtxLayout.Create(vtxType);
		const int vertexStride = vtxLayout.GetVertexSize();

		vector<BYTE> vertices(vtxCount * vertexStride);
		vector<WORD> indices(faceCount * 3);
		BYTE* pVertex = &vertices[0];
		WORD* pIndices = &indices[0];

		std::pair<int, int> info = { 0, 0 }; // vidx, iidx

		const int posOffset = vtxLayout.GetOffset("POSITION");
		const int normOffset = vtxLayout.GetOffset("NORMAL");
		const int colorOffset = vtxLayout.GetOffset("COLOR");
		const int texOffset = vtxLayout.GetOffset("TEXCOORD");

		int baseIdx = 0;
		const float inc = (MATH_PI * 2.0f) / slices;
		for (int i = 0; i < slices; ++i)
		{
			const Vector3 pos(sin(inc * i) * radius, 0.f,
				cos(inc * i) * radius);
			const Vector3 norm = pos.Normal();
			graphic::SetVertex(pVertex, posOffset, normOffset, colorOffset, texOffset
				, baseIdx, &(pos + Vector3(0, height, 0)), &norm, &color);
			baseIdx += vertexStride;

			graphic::SetVertex(pVertex, posOffset, normOffset, colorOffset, texOffset
				, baseIdx, &(pos - Vector3(0, height, 0)), &norm, &color);
			baseIdx += vertexStride;
		}

		// face
		int iidx = 0;
		for (int i = 0; i < slices; ++i)
		{
			const int idx0 = i * 2;
			const int idx1 = i * 2 + 1;
			const int idx2 = (i * 2 + 2) % (slices * 2);
			const int idx3 = (i * 2 + 3) % (slices * 2);

			indices[iidx++] = idx0;
			indices[iidx++] = idx1;
			indices[iidx++] = idx2;

			indices[iidx++] = idx2;
			indices[iidx++] = idx1;
			indices[iidx++] = idx3;
		}

		info.first = baseIdx;
		info.second = iidx;

		m_vtxBuff1.Create(renderer, vtxCount, vertexStride, pVertex);
		m_idxBuff1.Create(renderer, faceCount, (BYTE*)pIndices);
	}

	// generate half sphere
	{
		const int faceCount = ((slices + ((stacks - 1) * slices) * 2) * 1);
		const int vtxCount = faceCount * 3;
		cVertexLayout vtxLayout;
		vtxLayout.Create(vtxType);
		const int vertexStride = vtxLayout.GetVertexSize();

		vector<BYTE> vertices(vtxCount * vertexStride);
		vector<WORD> indices(faceCount * 3);
		BYTE* pVertex = &vertices[0];
		WORD* pIndices = &indices[0];

		std::pair<int, int> info = { 0, 0 }; // vidx, iidx

		// top cap
		info = GenerateHalfSphereMesh(pVertex, pIndices, info.first, info.second
			, vtxType, radius, stacks, slices, color, false, 0.f);

		m_vtxBuff2.Create(renderer, vtxCount, vertexStride, pVertex);
		m_idxBuff2.Create(renderer, faceCount, (BYTE*)pIndices);
	}

	m_vtxType = vtxType;
	return true;
}


void cCapsuleShape::Render(cRenderer &renderer)
{
	// nothing to do
	// call Render(renderer, radius, height, prentTm) function
}


// height: half height
void cCapsuleShape::Render(cRenderer& renderer, const float radius, const float height
	, const XMMATRIX& parentTm //= XMIdentity
)
{
	// render cylinder
	Matrix44 mat;
	mat.SetScale(Vector3(radius, height, radius));
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(mat.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	m_vtxBuff1.Bind(renderer);
	m_idxBuff1.Bind(renderer);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff1.GetFaceCount() * 3, 0, 0);
	//~

	// render half sphere, top
	Transform tfm0;
	tfm0.pos = Vector3(0, height, 0);
	tfm0.scale = Vector3(radius, radius, radius);
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm0.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	m_vtxBuff2.Bind(renderer);
	m_idxBuff2.Bind(renderer);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff2.GetFaceCount() * 3, 0, 0);
	//~

	// render half sphere, bottom
	Transform tfm1;
	tfm1.pos = Vector3(0, -height, 0);
	tfm1.scale = Vector3(radius, radius, radius);
	tfm1.rot.SetRotationX(MATH_PI);
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm1.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff2.GetFaceCount() * 3, 0, 0);
	//~
}


// generate half sphere
// return {vidx, iidx}
std::pair<int, int> cCapsuleShape::GenerateHalfSphereMesh(BYTE* vertices, WORD* indices
	, const int startVerticesIdx, const int startIndicesIdx
	, const int vtxType, const float radius, const int stacks, const int slices
	, const cColor& color, const bool isFlipY, const float yOffset)
{
	cVertexLayout vtxLayout;
	vtxLayout.Create(vtxType);

	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();

	int baseIdx = startVerticesIdx;
	int iidx = startIndicesIdx;
	int incIdxIdx = startVerticesIdx / vertexStride;
	const float rad90 = MATH_PI / 2.f;
	
	// stacks are ELEVATION so they count theta
	for (int t = 0; t < stacks; ++t)
	{		
		// x-y plane
		float theta1 = ((float)t / (float)stacks) * rad90; // 90 deg
		float theta2 = ((float)(t + 1) / (float)stacks) * rad90; // 90 deg

		// slices are ORANGE SLICES so the count azimuth
		for (int p = 0; p < slices; ++p)
		{
			// z-x plane
			float phi1 = ((float)p / (float)slices) * 2 * MATH_PI; // azimuth goes around 0 .. 2*PI
			float phi2 = (((float)p + 1) / (float)slices) * 2.f * MATH_PI;
			if (isFlipY)
			{
				phi1 = -phi1;
				phi2 = -phi2;
			}

			//phi1   phi2
			// |      |
			// 1------2 -- theta1
			// |\ _   |
			// |    \ |
			// 4------3 -- theta2
			//
			const float r1 = sin(theta1) * radius;
			const float r2 = sin(theta2) * radius;
			float y1 = cos(theta1) * radius;
			float y2 = cos(theta2) * radius;
			// update y offset
			if (isFlipY)
			{
				y1 = -y1;
				y2 = -y2;
			}

			const float z11 = cos(phi1) * r1;
			const float x11 = sin(phi1) * r1;
			const float z12 = cos(phi2) * r1;
			const float x12 = sin(phi2) * r1;

			const float z21 = cos(phi1) * r2;
			const float x21 = sin(phi1) * r2;
			const float z22 = cos(phi2) * r2;
			const float x22 = sin(phi2) * r2;

			Vector3 vertex1(x11, y1, z11);
			Vector3 vertex2(x12, y1, z12);
			Vector3 vertex3(x22, y2, z22);
			Vector3 vertex4(x21, y2, z21);

			Vector3 n1 = vertex1.Normal();
			Vector3 n2 = vertex2.Normal();
			Vector3 n3 = vertex3.Normal();
			Vector3 n4 = vertex4.Normal();

			// update y offset
			y1 = yOffset + y1;
			y2 = yOffset + y2;
			vertex1.y = y1;
			vertex2.y = y1;
			vertex3.y = y2;
			vertex4.y = y2;

			{
				graphic::SetVertex(vertices, posOffset, normOffset, colorOffset, texOffset
					, baseIdx, &vertex1, &n1, &color);
				baseIdx += vertexStride;
				graphic::SetVertex(vertices, posOffset, normOffset, colorOffset, texOffset
					, baseIdx, &vertex4, &n4, &color);
				baseIdx += vertexStride;
				graphic::SetVertex(vertices, posOffset, normOffset, colorOffset, texOffset
					, baseIdx, &vertex3, &n3, &color);
				baseIdx += vertexStride;

				indices[iidx++] = incIdxIdx++;
				indices[iidx++] = incIdxIdx++;
				indices[iidx++] = incIdxIdx++;
			}
			if (t > 0)
			{
				graphic::SetVertex(vertices, posOffset, normOffset, colorOffset, texOffset
					, baseIdx, &vertex1, &n1, &color);
				baseIdx += vertexStride;
				graphic::SetVertex(vertices, posOffset, normOffset, colorOffset, texOffset
					, baseIdx, &vertex3, &n3, &color);
				baseIdx += vertexStride;
				graphic::SetVertex(vertices, posOffset, normOffset, colorOffset, texOffset
					, baseIdx, &vertex2, &n2, &color);
				baseIdx += vertexStride;

				indices[iidx++] = incIdxIdx++;
				indices[iidx++] = incIdxIdx++;
				indices[iidx++] = incIdxIdx++;
			}
		}
	}

	return { baseIdx, iidx };
}
