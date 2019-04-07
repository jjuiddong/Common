//
// 2019-02-04, jjuiddong
// slidebar
//	- 2 cube, sliding cube scale
//
#pragma once


namespace graphic
{

	class cSlideBar : public cNode
	{
	public:
		cSlideBar();
		virtual ~cSlideBar();

		bool Create(cRenderer &renderer
			, const Vector3 &p0 = Vector3::Zeroes
			, const Vector3 &p1 = Vector3::Zeroes
			, const cColor &c0 = cColor::BLACK
			, const cColor &c1 = cColor::WHITE
			, const float width = 0.5f
			, const float slidePos = 0.5f );

		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;
		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;

		void SetSlidePos(const float p);
		void SetSlideBar(const Vector3 &p0, const Vector3 &p1, const float width, const float slidePos);
		void SetColor(const cColor &c0, const cColor &c1);
		void SetAlpha(const float alpha);
		void SetAnimation(const bool isAni, const bool aniVelocity);


	protected:
		void SetShape(const Vector3 &p0, const Vector3 &p1, const float width, const float slidePos);


	public:
		float m_slidePos; // 0 ~ 1, p0 -> p1
		cColor m_color[2];
		cCubeShape m_shape[2];
		Transform m_tfm[2];
		Vector3 m_pos[2]; // p0, p1
		float m_width;
		bool m_isAnimation; // slide animation
		float m_aniVelocity; //  1.f / sec
	};

}
