
#include "stdafx.h"
#include "billboard.h"


using namespace graphic;


cBillboard::cBillboard()
{
}

cBillboard::~cBillboard()
{
}


// 빌보드 초기화.
bool cBillboard::Create(cRenderer &renderer, const BILLBOARD_TYPE::TYPE type,
	const float width, const float height, 
	const Vector3 &pos, const string &textureFileName, const bool isSizePow2)
{
	if (!__super::Create(renderer, width, height, pos, textureFileName, isSizePow2))
		return false;

	m_type = type;
	return true;
}


void cBillboard::Rotate(const Vector3 &parentPos //= Vector3(0, 0, 0)
)
{
	Matrix44 mat;

	switch (m_type)
	{
	case BILLBOARD_TYPE::NONE:
		break;

	case BILLBOARD_TYPE::Y_AXIS:
	{
		// Y축 빌보드 행렬을 계산한다.
		Matrix44 view;
		view.SetView2(m_tm.GetPosition()+ parentPos, GetMainCamera()->GetEyePos(), Vector3(0, -1, 0));

		mat._11 = view._11;
		mat._13 = view._13;
		mat._31 = view._31;
		mat._33 = view._33;
	}
	break;

	case BILLBOARD_TYPE::ALL_AXIS:
	{
		// 모든 축에서 빌보드 행렬을 계산한다.
		Matrix44 view;
		view.SetView2(m_tm.GetPosition()+ parentPos, GetMainCamera()->GetEyePos(), Vector3(0, 1, 0));

		mat = view;
		mat._41 = mat._42 = mat._43 = 0;
	}
	break;

	case BILLBOARD_TYPE::ALL_AXIS_Z:
	{
		// 모든 축에서 빌보드 행렬을 계산한다.
		Matrix44 view;
		view.SetView2(m_tm.GetPosition()+ parentPos, GetMainCamera()->GetEyePos(), GetMainCamera()->GetUpVector());

		mat = view;
		mat._41 = mat._42 = mat._43 = 0;
	}
	break;
	}

	Matrix44 T;
	T.SetTranslate(GetTransform().GetPosition());
	SetTransform(mat.Transpose() * T);
}


// 화면에 출력.
void cBillboard::Render(cRenderer &renderer
	, const Vector3 &parentPos //= Vector3(0, 0, 0)
)
{
	Rotate(parentPos);
	Matrix44 tm;
	tm.SetPosition(parentPos);
	__super::Render(renderer, tm);
}


void cBillboard::RenderFactor(cRenderer &renderer
	, const Vector3 &parentPos //= Vector3(0, 0, 0)
)
{
	Rotate(parentPos);

	Matrix44 tm;
	tm.SetPosition(parentPos);
	__super::RenderFactor(renderer, tm);
}
