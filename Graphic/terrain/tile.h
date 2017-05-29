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

		bool Create(cRenderer &renderer
			, const string &name
			, const sRectf &rect
			, const float y=0, const float uvFactor=1.f
			, const Vector2 &uv0 = Vector2(0, 0), const Vector2 &uv1 = Vector2(1, 1)
			);
		void Update(cRenderer &renderer, const float deltaSeconds);
		void PreRender(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		void Render(cRenderer &renderer
			, const Matrix44 &mVPT, const Matrix44 &mLVP, const Matrix44 &mLV, 
			cShadowMap *shadowMap = NULL, const Matrix44 &tm = Matrix44::Identity);
		void Render2(cRenderer &renderer
			, const Matrix44 &mLightView, const Matrix44 &mLightProj, const Matrix44 &mLightTT,
			cShadowMap *shadowMap = NULL, const Matrix44 &tm = Matrix44::Identity);
		float CullingTest(const cFrustum &frustum, const bool isModel = true);
		bool AddModel(cModel2 *model);
		bool RemoveModel(cModel2 *model);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Clear();


	public:
		bool m_isShow;
		bool m_isShadow;
		bool m_isDbgRender;
		string m_name;
		cGrid3 m_ground;
		Matrix44 m_tm;
		cBoundingBox m_boundingBox;
		vector<cModel2*> m_models;
		set<cShader*> m_shaders;
		
		cDbgBox2 m_dbgTile;
	};

}
