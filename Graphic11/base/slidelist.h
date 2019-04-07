//
// 2019-02-03, jjuiddong
// slidebar list
//	- instancing
//
#pragma once


namespace graphic
{

	class cSlideList : public cNode
	{
	public:
		struct sSlideInfo {
			int id;
			float spos; // slide pos
			float apos; // animation pos, length
			float len; // distance p0 - p1, for animation
			Vector3 pos[2]; //p0, p1
			Transform tfm[3];

			bool operator==(const sSlideInfo &rhs) const {
				return id == rhs.id;
			}
		};

		cSlideList();
		virtual ~cSlideList();

		bool Create(cRenderer &renderer
			, const int reserveSlideCount = 10
			, const cColor &c0 = cColor::BLACK
			, const cColor &c1 = cColor::WHITE
			, const float barSize = 0.5f
			, const float width = 0.5f);

		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;
		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;
		int Add(const int id, const Vector3 &p0, const Vector3 &p1, const float slidePos);
		bool Remove(const int id);
		bool SetSlide(const int id, const Vector3 &p0, const Vector3 &p1, const float slidePos);
		void SetAlpha(const float alpha);
		void SetAnimation(const bool isAni, const bool aniVelocity);
		void Clear();


	protected:
		void SetSlideShpae(sSlideInfo &slide, const Vector3 &p0, const Vector3 &p1
			, const float barSize, const float width, const float slidePos);


	public:
		cColor m_color[2];
		cCubeShape m_shape[3];
		float m_barSize;
		float m_width;
		bool m_isAnimation; // slide animation
		float m_aniVelocity; //  1.f / sec
		vector<sSlideInfo> m_slides;
	};

}
