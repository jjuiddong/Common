//
// 2017-05-13, jjuiddong
// terrain tile
//
#pragma once


namespace graphic
{

	class cTile : public cNode2
	{
	public:
		cTile();
		virtual ~cTile();

		virtual bool Create(cRenderer &renderer
			, const int id
			, const Str64 &name
			, const sRectf &rect
			, const float y=0, const float uvFactor=1.f
			, const Vector2 &uv0 = Vector2(0, 0), const Vector2 &uv1 = Vector2(1, 1)
			);

		virtual bool Create(cRenderer &renderer
			, const int id
			, const Str64 &name
			, const Vector3 &dim
			, const Transform &transform
			, const float y = 0, const float uvFactor = 1.f
			, const Vector2 &uv0 = Vector2(0, 0), const Vector2 &uv1 = Vector2(1, 1)
		);

		void UpdateShader(const Matrix44 *mLightView, const Matrix44 *mLightProj, const Matrix44 *mLightTT
			, cShadowMap *shadowMap = NULL, const int shadowMapCount = 0);
		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;
		virtual void PreRender(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		virtual bool Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity, const int flags = 1) override;
		virtual void DebugRender(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);

		virtual float CullingTest(const cFrustum &frustum, const Matrix44 &tm = Matrix44::Identity, const bool isModel = true);
		virtual bool AddChild(cNode2 *node) override;
		virtual bool RemoveChild(cNode2 *rmNode, const bool rmInstance = true) override;
		virtual const cNode2* FindNode(const int id) const override;
		void UpdatePosition(const sRectf &rect);

		virtual void LostDevice();
		virtual void ResetDevice(cRenderer &renderer);
		virtual void Clear();


	public:
		Vector2i m_location; // Tile Location row, col
		cGrid3 m_ground;
		set<cShader*> m_shaders; // reference

		bool m_isDbgRender;	
		cDbgBox2 m_dbgTile;
	};

}
