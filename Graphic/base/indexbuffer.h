// 인덱스 버퍼 클래스
// LPDIRECT3DINDEXBUFFER9 인터페이스를 쉽게 쓰기 위해 만들어짐.
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
		void* Lock();
		void Unlock();
		void Bind(cRenderer &renderer) const;
		int GetFaceCount() const;
		void Clear();

		void Set(cRenderer &renderer, cIndexBuffer &rhs);
		//cIndexBuffer& operator=(cIndexBuffer &rhs);


	private:
		LPDIRECT3DINDEXBUFFER9 m_pIdxBuff; // 인덱스 버퍼
		int m_faceCount;
	};


	inline int cIndexBuffer::GetFaceCount() const { return m_faceCount; }
}
