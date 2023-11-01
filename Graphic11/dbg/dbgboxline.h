//
// 2023-10-31, jjuiddong
// Debug Box Line
//
#pragma once


namespace graphic
{
	using namespace common;
	class cRenderer;

	class cDbgBoxLine
	{
	public:
		cDbgBoxLine();
		cDbgBoxLine(cRenderer &renderer, const cBoundingBox &bbox, const cColor &color=cColor::BLACK);

		void Create(cRenderer &renderer, const cColor &color = cColor::BLACK);
		void Create(cRenderer &renderer, const cBoundingBox &bbox, const cColor &color = cColor::BLACK);
		void SetBox(const cBoundingBox &bbox);
		void SetBox(const Transform &tfm);
		void SetBox(cRenderer &renderer, const Vector3 vertices[8], const cColor &color = cColor::BLACK);
		void SetColor(const cColor &color);
		void Render(cRenderer &renderer, const XMMATRIX &tm=XMIdentity);
		void RenderInstancing(cRenderer &renderer
			, const int count
			, const XMMATRIX *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1
		);
		XMMATRIX GetTransform() const;
		void SetTechnique(const char *techniqName);


	protected:
		void UpdateLine(cRenderer& renderer);


	public:
		cDbgLineList m_lines;
		cBoundingBox m_boundingBox;
		cColor m_color;
		bool m_isUpdate; // refresh line?
	};

}
