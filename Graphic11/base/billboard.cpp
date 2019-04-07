
#include "stdafx.h"
#include "billboard.h"

using namespace graphic;


cBillboard::cBillboard()
	//: m_dynScaleMin(1.f)
	//, m_dynScaleMax(2.5f)
	: m_dynScaleMin(0.5f)
	, m_dynScaleMax(200.5f)
	, m_dynScaleAlpha(1.f)
{
}

cBillboard::~cBillboard()
{
}


// 빌보드 초기화.
bool cBillboard::Create(cRenderer &renderer, const BILLBOARD_TYPE::TYPE type,
	const float width, const float height, 
	const Vector3 &pos
	, const StrPath &textureFileName //=""
	, const bool isSizePow2 //=true
	, const float dynScaleMin //= 0.5f
	, const float dynScaleMax //= 200.5f
	, const float dynScaleAlpha //= 1.f
)
{
	if (!__super::Create(renderer, width, height, pos
		, eVertexType::POSITION | eVertexType::TEXTURE0, textureFileName, true))
		return false;

	m_type = type;
	m_scale = Vector3(1, 1, 1);
	m_transform.pos = pos;
	m_dynScaleMin = dynScaleMin;
	m_dynScaleMax = dynScaleMax;
	m_dynScaleAlpha = dynScaleAlpha;
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
		Matrix44 view = GetMainCamera().GetViewMatrix();

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
		Matrix44 view = GetMainCamera().GetViewMatrix();
		mat = view;
		mat._41 = mat._42 = mat._43 = 0;
		mat.Transpose();
	}
	break;

	case BILLBOARD_TYPE::ALL_AXIS_Z:
	{
		// 모든 축에서 빌보드 행렬을 계산한다.
		Matrix44 view = GetMainCamera().GetViewMatrix();

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
		const Vector3 scale = Vector3(1,1,1)*min(m_dynScaleMax
			, max(m_dynScaleMin, (len / 100.f) * m_dynScaleAlpha));
		m_scale = scale;

		Matrix44 view = GetMainCamera().GetViewMatrix();
		mat = view;
		mat._41 = mat._42 = mat._43 = 0;
		mat.Transpose();
	}
	break;
	}

	m_transform.rot = mat.GetQuaternion();
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

	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	Transform tfm = m_transform;
	tfm.scale *= m_scale;

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbMaterial.Update(renderer, 2);

	if (m_texture)
		m_texture->Bind(renderer, 0);

	renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
	m_shape.Render(renderer);
	renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);

	renderer.GetDevContext()->RSSetState(states.CullCounterClockwise());
	return true;
}
