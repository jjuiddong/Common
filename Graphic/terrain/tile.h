//
// 2017-05-13, jjuiddong
// terrain tile
//
#pragma once


namespace graphic
{

	class cTile
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
			, const Matrix44 &tm
			, const float y = 0, const float uvFactor = 1.f
			, const Vector2 &uv0 = Vector2(0, 0), const Vector2 &uv1 = Vector2(1, 1)
		);

		virtual void Update(cRenderer &renderer, const float deltaSeconds);
		virtual void PreRender(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);

		//virtual void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		
		//virtual void Render(cRenderer &renderer
		//	, const Matrix44 &mVPT, const Matrix44 &mLVP
		//	, cShadowMap *shadowMap = NULL
		//	, const Matrix44 &tm = Matrix44::Identity);

		//virtual void Render2(cRenderer &renderer
		//	, const Matrix44 &mLightView, const Matrix44 &mLightProj, const Matrix44 &mLightTT
		//	, cShadowMap *shadowMap = NULL
		//	, const Matrix44 &tm = Matrix44::Identity);

		virtual void Render2(cRenderer &renderer
			, const Matrix44 *mLightView, const Matrix44 *mLightProj, const Matrix44 *mLightTT
			, cShadowMap *shadowMap=NULL, const int shadowMapCount=0
			, const Matrix44 &tm = Matrix44::Identity
			, const int flags = 1);

		virtual void DebugRender(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);

		virtual float CullingTest(const cFrustum &frustum, const bool isModel = true);
		virtual bool AddModel(cModel2 *model);
		virtual bool RemoveModel(cModel2 *model);
		virtual cModel2* FindModel(const int modelId);
		virtual bool AddChild(cTile *tile);
		virtual bool RemoveChild(const int id);
		virtual cTile* FindChild(const int id);
		virtual void LostDevice();
		virtual void ResetDevice(cRenderer &renderer);
		virtual void Clear();


	public:
		int m_id;
		Str64 m_name;
		cGrid3 m_ground;
		Matrix44 m_tm;
		cBoundingBox m_boundingBox; // Local Space
		vector<cModel2*> m_models;
		set<cShader*> m_shaders;
		vector<cTile*> m_children;
		bool m_isEnable;
		bool m_isShow;
		bool m_isShadow;
		bool m_isDbgRender;
	
		cDbgBox2 m_dbgTile;
	};

}
