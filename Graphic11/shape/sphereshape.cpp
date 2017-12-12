
#include "stdafx.h"
#include "sphereshape.h"

using namespace graphic;

cSphereShape::cSphereShape()
{
}

cSphereShape::~cSphereShape()
{
}


bool cSphereShape::Create(cRenderer &renderer, const float radius, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= cColor::BLACK
)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return false;

	m_numSlices = slices;
	m_numStacks = stacks;

	const int vtxCount = (slices) * (((stacks - 2) * 4) + 6);
	const int faceCount = ((slices - 1) * 6) * stacks;

	cVertexLayout vtxLayout;
	vtxLayout.Create(vtxType);
	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();

	vector<BYTE> vertices(vtxCount * vertexStride);
	vector<WORD> indices(faceCount * 3);
	BYTE *pVertex = &vertices[0];
	WORD *pIndices = &indices[0];

	int idxIdx = 0;
	int vtxIdx = 0;
	const Vector4 vColor = color.GetColor();

	for (int t = 0; t < stacks; t++) // stacks are ELEVATION so they count theta
	{
		const float theta1 = ((float)t / (float)stacks) * MATH_PI;
		const float theta2 = ((float)(t + 1) / (float)stacks) * MATH_PI;
		const float v1 = (float)t / (float)stacks;
		const float v2 = (float)(t + 1) / (float)stacks;

		for (int p = 0; p < slices; p++) // slices are ORANGE SLICES so the count azimuth
		{
			const float phi1 = ((float)p / (float)slices) * 2 * MATH_PI; // azimuth goes around 0 .. 2*PI
			const float phi2 = ((float)(p + 1) / (float)slices) * 2 * MATH_PI;
			const float u1 = (float)p / (float)slices;
			const float u2 = (float)(p + 1) / (float)slices;

			//phi1   phi2
			// |      |
			// 1------2 -- theta1
			// |\ _   |
			// |    \ |
			// 4------3 -- theta2
			//

			const float r1 = sin(theta1) * radius;
			const float r2 = sin(theta2) * radius;
			const float y1 = cos(theta1) * radius;
			const float y2 = cos(theta2) * radius;

			const float x11 = cos(phi1) * r1;
			const float z11 = sin(phi1) * r1;
			const float x12 = cos(phi2) * r1;
			const float z12 = sin(phi2) * r1;

			const float x21 = cos(phi1) * r2;
			const float z21 = sin(phi1) * r2;
			const float x22 = cos(phi2) * r2;
			const float z22 = sin(phi2) * r2;

			const Vector3 vertex1(x11, y1, z11);
			const Vector3 vertex2(x12, y1, z12);
			const Vector3 vertex3(x22, y2, z22);
			const Vector3 vertex4(x21, y2, z21);

			const Vector3 n1 = vertex1.Normal();
			const Vector3 n2 = vertex2.Normal();
			const Vector3 n3 = vertex3.Normal();
			const Vector3 n4 = vertex4.Normal();

			// facing out
			if (t == 0) // top cap
			{
				if (vtxType & eVertexType::POSITION)
					*(Vector3*)(pVertex + posOffset) = vertex1;
				if (vtxType & eVertexType::COLOR)
					*(Vector4*)(pVertex + colorOffset) = vColor;
				if (vtxType & eVertexType::NORMAL)
					*(Vector3*)(pVertex + normOffset) = n1;
				if (vtxType & eVertexType::TEXTURE0)
					*(Vector2*)(pVertex + texOffset) = Vector2(u1,v1);
				//pVertex.n = n1;
				//pVertex.u = u1;
				//pVertex.v = v1;
				pVertex += vertexStride;

				if (vtxType & eVertexType::POSITION)
					*(Vector3*)(pVertex + posOffset) = vertex3;
				if (vtxType & eVertexType::COLOR)
					*(Vector4*)(pVertex + colorOffset) = vColor;
				if (vtxType & eVertexType::NORMAL)
					*(Vector3*)(pVertex + normOffset) = n3;
				if (vtxType & eVertexType::TEXTURE0)
					*(Vector2*)(pVertex + texOffset) = Vector2(u2, v2);
				//pVertex.p = vertex3;
				//pVertex.c = vColor;
				//pVertex.n = n3;
				//pVertex.u = u2;
				//pVertex.v = v2;
				pVertex += vertexStride;

				if (vtxType & eVertexType::POSITION)
					*(Vector3*)(pVertex + posOffset) = vertex4;
				if (vtxType & eVertexType::COLOR)
					*(Vector4*)(pVertex + colorOffset) = vColor;
				if (vtxType & eVertexType::NORMAL)
					*(Vector3*)(pVertex + normOffset) = n4;
				if (vtxType & eVertexType::TEXTURE0)
					*(Vector2*)(pVertex + texOffset) = Vector2(u1, v2);
				//pVertex.p = vertex4;
				//pVertex.c = vColor;
				//pVertex.n = n4;
				//pVertex.u = u1;
				//pVertex.v = v2;
				pVertex += vertexStride;

				*pIndices++ = idxIdx++;
				*pIndices++ = idxIdx++;
				*pIndices++ = idxIdx++;
			}
			else if (t + 1 == stacks) //end cap
			{
				if (vtxType & eVertexType::POSITION)
					*(Vector3*)(pVertex + posOffset) = vertex3;
				if (vtxType & eVertexType::COLOR)
					*(Vector4*)(pVertex + colorOffset) = vColor;
				if (vtxType & eVertexType::NORMAL)
					*(Vector3*)(pVertex + normOffset) = n3;
				if (vtxType & eVertexType::TEXTURE0)
					*(Vector2*)(pVertex + texOffset) = Vector2(u2, v2);
				//pVertex.p = vertex3;
				//pVertex.c = vColor;
				//pVertex.n = n3;
				//pVertex.u = u2;
				//pVertex.v = v2;
				pVertex += vertexStride;

				if (vtxType & eVertexType::POSITION)
					*(Vector3*)(pVertex + posOffset) = vertex1;
				if (vtxType & eVertexType::COLOR)
					*(Vector4*)(pVertex + colorOffset) = vColor;
				if (vtxType & eVertexType::NORMAL)
					*(Vector3*)(pVertex + normOffset) = n1;
				if (vtxType & eVertexType::TEXTURE0)
					*(Vector2*)(pVertex + texOffset) = Vector2(u1, v1);
				//pVertex.p = vertex1;
				//pVertex.c = vColor;
				//pVertex.n = n1;
				//pVertex.u = u1;
				//pVertex.v = v1;
				pVertex += vertexStride;

				if (vtxType & eVertexType::POSITION)
					*(Vector3*)(pVertex + posOffset) = vertex2;
				if (vtxType & eVertexType::COLOR)
					*(Vector4*)(pVertex + colorOffset) = vColor;
				if (vtxType & eVertexType::NORMAL)
					*(Vector3*)(pVertex + normOffset) = n2;
				if (vtxType & eVertexType::TEXTURE0)
					*(Vector2*)(pVertex + texOffset) = Vector2(u2, v1);
				//pVertex.p = vertex2;
				//pVertex.c = vColor;
				//pVertex.n = n2;
				//pVertex.u = u2;
				//pVertex.v = v1;
				pVertex += vertexStride;


				*pIndices++ = idxIdx++;
				*pIndices++ = idxIdx++;
				*pIndices++ = idxIdx++;
			}
			else
			{
				// body, facing OUT:
				if (vtxType & eVertexType::POSITION)
					*(Vector3*)(pVertex + posOffset) = vertex1;
				if (vtxType & eVertexType::COLOR)
					*(Vector4*)(pVertex + colorOffset) = vColor;
				if (vtxType & eVertexType::NORMAL)
					*(Vector3*)(pVertex + normOffset) = n1;
				if (vtxType & eVertexType::TEXTURE0)
					*(Vector2*)(pVertex + texOffset) = Vector2(u1, v1);
				//pVertex.p = vertex1;
				//pVertex.c = vColor;
				//pVertex.n = n1;
				//pVertex.u = u1;
				//pVertex.v = v1;
				pVertex += vertexStride;

				if (vtxType & eVertexType::POSITION)
					*(Vector3*)(pVertex + posOffset) = vertex2;
				if (vtxType & eVertexType::COLOR)
					*(Vector4*)(pVertex + colorOffset) = vColor;
				if (vtxType & eVertexType::NORMAL)
					*(Vector3*)(pVertex + normOffset) = n2;
				if (vtxType & eVertexType::TEXTURE0)
					*(Vector2*)(pVertex + texOffset) = Vector2(u2, v1);
				//pVertex.p = vertex2;
				//pVertex.c = vColor;
				//pVertex.n = n2;
				//pVertex.u = u2;
				//pVertex.v = v1;
				pVertex += vertexStride;

				if (vtxType & eVertexType::POSITION)
					*(Vector3*)(pVertex + posOffset) = vertex3;
				if (vtxType & eVertexType::COLOR)
					*(Vector4*)(pVertex + colorOffset) = vColor;
				if (vtxType & eVertexType::NORMAL)
					*(Vector3*)(pVertex + normOffset) = n3;
				if (vtxType & eVertexType::TEXTURE0)
					*(Vector2*)(pVertex + texOffset) = Vector2(u2, v2);
				//pVertex.p = vertex3;
				//pVertex.c = vColor;
				//pVertex.n = n3;
				//pVertex.u = u2;
				//pVertex.v = v2;
				pVertex += vertexStride;

				if (vtxType & eVertexType::POSITION)
					*(Vector3*)(pVertex + posOffset) = vertex4;
				if (vtxType & eVertexType::COLOR)
					*(Vector4*)(pVertex + colorOffset) = vColor;
				if (vtxType & eVertexType::NORMAL)
					*(Vector3*)(pVertex + normOffset) = n4;
				if (vtxType & eVertexType::TEXTURE0)
					*(Vector2*)(pVertex + texOffset) = Vector2(u1, v2);
				//pVertex.p = vertex4;
				//pVertex.c = vColor;
				//pVertex.n = n4;
				//pVertex.u = u1;
				//pVertex.v = v2;
				pVertex += vertexStride;

				const int base = idxIdx;

				*pIndices++ = base;
				*pIndices++ = base + 1;
				*pIndices++ = base + 3;

				*pIndices++ = base + 1;
				*pIndices++ = base + 2;
				*pIndices++ = base + 3;

				idxIdx += 4;
			}
		}
	}

	m_vtxBuff.Create(renderer, vtxCount, vertexStride, &vertices[0]);
	m_idxBuff.Create(renderer, faceCount, (BYTE*)&indices[0]);
	m_vtxType = vtxType;
	return true;
}


void cSphereShape::Render(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 3, 0, 0);
}
