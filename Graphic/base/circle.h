#pragma once


namespace graphic
{

	class cCircle
	{
	public:
		cCircle();
		virtual ~cCircle();

		bool Create(cRenderer &renderer, const Vector3 &center, const float radius, const int slice);
		void Render(cRenderer &renderer);


	private:
		float m_radius;
		Matrix44 m_tm;
		cVertexBuffer m_vtxBuff;
	};

}
