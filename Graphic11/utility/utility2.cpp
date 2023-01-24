
#include "stdafx.h"
#include "utility2.h"


using namespace graphic;


// return vertex stride size (byte size)
// vtxType: composite of eVertexType
//int graphic::GetVertexStride(const int vtxType)
//{
//	int stride = 0;
//	if (vtxType & eVertexType::POSITION)
//		stride += sizeof(float) * 3;
//	if (vtxType & eVertexType::POSITION_RHW)
//		stride += sizeof(float) * 3;
//	if (vtxType & eVertexType::NORMAL)
//		stride += sizeof(float) * 3;
//	if (vtxType & eVertexType::TEXTURE0)
//		stride += sizeof(float) * 2;
//	if (vtxType & eVertexType::TEXTURE1)
//		stride += sizeof(float) * 2;
//	if (vtxType & eVertexType::COLOR)
//		stride += sizeof(float) * 4; // RGBA
//	if (vtxType & eVertexType::TANGENT)
//		stride += sizeof(float) * 3;
//	if (vtxType & eVertexType::BINORMAL)
//		stride += sizeof(float) * 3;
//	if (vtxType & eVertexType::BLENDINDICES)
//		stride += sizeof(BYTE) * 4;
//	if (vtxType & eVertexType::BLENDWEIGHT)
//		stride += sizeof(BYTE) * 4;
//	return stride;
//}


// set vertex data
bool graphic::SetVertex(BYTE* vertices, const int posOffset, const int normOffset
	, const int colorOffset, const int texOffset, const int baseIdx
	, const Vector3* pos
	, const Vector3* n //= nullptr
	, const cColor* color //= nullptr
	, const Vector2* uv //= nullptr
)
{
	if (pos && (posOffset >= 0))
		*(Vector3*)(vertices + baseIdx + posOffset) = *pos;
	if (n && (normOffset >= 0))
		*(Vector3*)(vertices + baseIdx + normOffset) = *n;
	if (color && (colorOffset >= 0))
	{
		const Vector4 vColor = color->GetColor();
		*(Vector4*)(vertices + baseIdx + colorOffset) = vColor;
	}
	if (uv && (texOffset >= 0))
		*(Vector2*)(vertices + baseIdx + texOffset) = *uv;
	return true;
}

