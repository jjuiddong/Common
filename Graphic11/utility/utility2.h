//
// 2023-01-21, jjuiddong
// Graphic Utility2
//
#pragma once


namespace graphic
{
	using namespace common;

	//int GetVertexStride(const int vtxType); use cVertexLayout

	bool SetVertex(BYTE* vertices, const int posOffset, const int normOffset
		, const int colorOffset, const int texOffset, const int baseIdx
		, const Vector3* pos
		, const Vector3* n = nullptr
		, const cColor* color = nullptr
		, const Vector2* uv = nullptr
	);

}
