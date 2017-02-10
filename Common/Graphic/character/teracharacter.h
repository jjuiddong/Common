 // 테라 게임 캐릭터 클래스.
//
#pragma once

#include "character.h"


namespace graphic
{
	class cRenderer;

	namespace TERA_MODEL
	{
		enum TYPE
		{
			FACE, 
			HAIR,
			BODY,
			HAND,
			LEG,
			TAIL,
			MAX_TYPE
		};
	}


	class cTeraCharacter : public cCharacter
	{
	public:
		cTeraCharacter();
		virtual ~cTeraCharacter();

		bool Create( cRenderer &renderer,
			const string &faceModelFileName,
			const string &hairModelFileName,
			const string &bodyModelFileName,
			const string &handModelFileName,
			const string &legModelFileName,
			const string &tailModelFileName );

		virtual bool SetAnimation( const string &aniFileName, const int nAniFrame=0,  
			const bool isLoop=false, const bool isBlend=true ) override;
		void SetBodyModel(cRenderer &renderer, const string &fileName);
		void SetHandModel(cRenderer &renderer, const string &fileName);
		void SetLegModel(cRenderer &renderer, const string &fileName);
		void SetFaceModel(cRenderer &renderer, const string &fileName);
		void SetHairModel(cRenderer &renderer, const string &fileName);
		void SetTailModel(cRenderer &renderer, const string &fileName);

		virtual bool Update(const float deltaSeconds) override;
		virtual void Render(cRenderer &renderer, const Matrix44 &tm) override;


	private:
		cModel *m_models[ TERA_MODEL::MAX_TYPE];

		cBoneNode *m_bodyNeckNode; // reference
		cBoneNode *m_bodyFaceNode; // reference
		cBoneNode *m_faceFaceNode; // reference

		cBoneNode *m_faceNeckNode; // reference
		cBoneNode *m_hairHairNode; // reference

	};

}
