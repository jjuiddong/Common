#pragma once

#include "character.h"


namespace graphic
{

	class cArchebladeCharacter : public cCharacter
	{
	public:
		cArchebladeCharacter(const int id);
		virtual ~cArchebladeCharacter();

		virtual bool Create(cRenderer &renderer, const string &modelName, MODEL_TYPE::TYPE type = MODEL_TYPE::AUTO) override;
		void LoadWeapon(cRenderer &renderer, const string &fileName);
		virtual bool Update(const float deltaSeconds) override;
		virtual void Render(cRenderer &renderer, const Matrix44 &tm) override;

		// debug ¿ë ÇÔ¼ö.
		void SetRenderWeaponBoundingBox(const bool isRenderBoundingBox);


	private:
		cModel *m_weapon;
		cBoneNode *m_weaponNode1; // reference
		cBoneNode *m_weaponNode2; // reference
		cBoneNode *m_weaponNode3; // reference
		cBoneNode *m_weaponBoneNode1; // reference
		cBoneNode *m_weaponBoneNode2; // reference
		cBoneNode *m_weaponBoneNode3; // reference
	};

}
