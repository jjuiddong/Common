//
// 2017-07-27, jjuiddong
// Upgrade Dx9 - Dx11
// IndexBuffer Class
//
#pragma once


namespace graphic
{
	class cRenderer;

	class cIndexBuffer
	{
	public:
		cIndexBuffer();
		virtual ~cIndexBuffer();

		bool Create(cRenderer &renderer, int faceCount);
		bool Create(cRenderer &renderer, int faceCount, WORD *indices);
		bool Create2(cRenderer &renderer, const int primitiveCount, const int primitiveSize);
		void* Lock();
		void Unlock();
		void Bind(cRenderer &renderer) const;
		int GetFaceCount() const;
		void Clear();

		void Set(cRenderer &renderer, cIndexBuffer &rhs);


	public:
		ID3D11Buffer *m_idxBuff;
		int m_faceCount;
	};


	inline int cIndexBuffer::GetFaceCount() const { return m_faceCount; }
}
