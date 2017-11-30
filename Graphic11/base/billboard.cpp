
#include "stdafx.h"
#include "billboard.h"


using namespace graphic;


cBillboard::cBillboard()
	: m_dynScaleMin(1.f)
	, m_dynScaleMax(2.5f)
{
}

cBillboard::~cBillboard()
{
}


// 빌보드 초기화.
bool cBillboard::Create(cRenderer &renderer, const BILLBOARD_TYPE::TYPE type,
	const float width, const float height, 
	const Vector3 &pos, const StrPath &textureFileName, const bool isSizePow2)
{
	if (!__super::Create(renderer, width, height, pos
		, eVertexType::POSITION | eVertexType::TEXTURE, textureFileName, true))
		return false;

	m_type = type;
	m_scale = m_transform.scale;
	m_transform.pos = pos;
	return true;
}


void cBillboard::Rotate()
{
	if (GetMainCamera().Is2DMode())
	{
		// 직교투영 꽁수처리, jjuiddong
		// 2017-11-10
		m_transform.pos.y = GetMainCamera().GetEyePos().y - 50;
		m_transform.rot.SetRotationArc(Vector3(0, 1, 0), Vector3(0, 0, 1));
		Quaternion q; // Camera Roll Rotation
		q.SetRotationArc(Vector3(0, 0, 1), GetMainCamera().m_up);
		m_transform.rot *= q;
		m_transform.scale = m_scale * 15;
		return;
	}

	Matrix44 mat;

	switch (m_type)
	{
	case BILLBOARD_TYPE::NONE:
		break;

	case BILLBOARD_TYPE::Y_AXIS:
	{
		// Y축 빌보드 행렬을 계산한다.
		Matrix44 view;
		view.SetView2(m_transform.pos, GetMainCamera().GetEyePos(), Vector3(0, 1, 0));

		mat._11 = view._11;
		mat._13 = view._13;
		mat._31 = view._31;
		mat._33 = view._33;
		mat.Transpose();
	}
	break;

	case BILLBOARD_TYPE::ALL_AXIS:
	{
		// 모든 축에서 빌보드 행렬을 계산한다.
		Matrix44 view;
		view.SetView2(m_transform.pos, GetMainCamera().GetEyePos(), Vector3(0, 1, 0));

		mat = view;
		mat._41 = mat._42 = mat._43 = 0;
		mat.Transpose();
	}
	break;

	case BILLBOARD_TYPE::ALL_AXIS_Z:
	{
		// 모든 축에서 빌보드 행렬을 계산한다.
		Matrix44 view;
		view.SetView2(m_transform.pos, GetMainCamera().GetEyePos(), GetMainCamera().GetUpVector());

		mat = view;
		mat._41 = mat._42 = mat._43 = 0;
		mat.Transpose();
	}
	break;


	case BILLBOARD_TYPE::DYN_SCALE:
	{
		// Fixed Scale Model
		Vector3 pos = m_transform.pos;
		const float len = (pos - GetMainCamera().GetEyePos()).Length();
		//const Vector3 scale = m_scale * min(1.5f, max(1.f, len / 50.f));
		const Vector3 scale = m_scale * min(m_dynScaleMax, max(m_dynScaleMin, len / 100.f));

		Matrix44 S;
		S.SetScale(scale);

		Matrix44 view;
		view.SetView2(m_transform.pos, GetMainCamera().GetEyePos(), Vector3(0, 1, 0));

		mat = view;
		mat._41 = mat._42 = mat._43 = 0;
		mat.Transpose();
		m_transform.scale = scale;
	}
	break;
	}

	Matrix44 R;
	R.SetRotationY(ANGLE2RAD(180)); // treaky code, didn't understand
	m_transform.rot = (R * mat).GetQuaternion();
	m_normal = (GetMainCamera().GetEyePos() - m_transform.pos).Normal();
}


// 화면에 출력.
bool cBillboard::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	Rotate();
	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(states.CullNone());
	__super::Render(renderer, parentTm, flags);
	renderer.GetDevContext()->RSSetState(states.CullCounterClockwise());
	return true;
}
