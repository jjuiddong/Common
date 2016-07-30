#pragma once


namespace graphic
{

	class cRigidMesh : public cMesh
	{
	public:
		cRigidMesh(cRenderer &renderer, const int id, const sRawMesh &raw);
		virtual ~cRigidMesh();

		void LoadAnimation( const sRawAni &rawAni );
		virtual bool Update(const float deltaSeconds) override;
		virtual void Render(cRenderer &renderer, const Matrix44 &parentTm) override;
		//virtual void RenderShader( const Matrix44 &parentTm ) override;

		void CreateBoundingBox(OUT cCube &out);
		void RenderBoundingBox(cRenderer &renderer, const Matrix44 &tm);
		const cCube& GetBoundingBox() const;


	private:
		// 애니메이션
		cTrack *m_track;
		int m_aniStart;
		int m_aniEnd;
		int m_aniFrame;
		float m_aniTime;
	};

}
