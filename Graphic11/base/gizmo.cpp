
#include "stdafx.h"
#include "gizmo.h"

using namespace graphic;


cGizmo::cGizmo()
	: cNode(common::GenerateId(), "gizmo", eNodeType::MODEL)
	, m_type(eGizmoEditType::TRANSLATE)
	, m_axisType(eGizmoEditAxis::X)
 	, m_transformType(eGizmoTransform::LOCAL)
	, m_isKeepEdit(false)
	, m_controlNode(NULL)
{
	ZeroMemory(m_pick, sizeof(m_pick));
}

cGizmo::~cGizmo()
{
}


bool cGizmo::Create(cRenderer &renderer)
{
	m_quad.Create(renderer, 1.f, 1.f, Vector3(0,0,0)
		, eVertexType::POSITION | eVertexType::COLOR);

	const int stack = 40;
	const float radius = 2.f;
	const float circleR = 0.08f;
	m_torus.Create(renderer, radius, radius- circleR*2, stack, 20);
	for (int i=0; i < 3; ++i)
		m_arrow[i].Create(renderer, Vector3(0, 0, 0), Vector3(1, 0, 0), 1.f, true);

	// Create Torus Picking BoundingBox	
	m_ringBbox.reserve(stack);
	const float incAngle = (MATH_PI * 2) / stack;
	const float bboxRadius = radius - circleR;
	float angle = 0;
	for (int i = 0; i < stack; ++i)
	{
		cBoundingBox bbox;
		Vector3 pos1(cos(angle)*bboxRadius, 0, sin(angle)*bboxRadius);
		Vector3 pos2(cos(angle + incAngle)*bboxRadius, 0, sin(angle + incAngle)*bboxRadius);
		bbox.SetLineBoundingBox(pos1, pos2, circleR);
		m_ringBbox.push_back(bbox);
		angle += incAngle;
	}

	return true;
}


void cGizmo::SetTransformType(const eGizmoTransform::Enum type)
{
	m_transformType = type;
}


// local space -> world space
void cGizmo::SetControlNode(cNode *node)
{
	if (!node)
	{
		Cancel();
		return;
	}

	m_controlNode = node;
	const Matrix44 worldTm = node->GetWorldMatrix();
	m_transform.pos = worldTm.GetPosition();

	// location between node and eye pos
	Vector3 v = worldTm.GetPosition() - GetMainCamera().GetEyePos();
	m_transform.pos = v.Normal() * 3 + GetMainCamera().GetEyePos();
}


// world space -> local space
void cGizmo::UpdateNodeTransform(const Transform &transform)
{
	RET2(!m_controlNode);

	if (eGizmoTransform::LOCAL == m_transformType)
	{
		switch (m_type)
		{
		case eGizmoEditType::TRANSLATE: m_controlNode->m_transform.pos += transform.pos; break;
		case eGizmoEditType::SCALE: m_controlNode->m_transform.scale *= transform.scale; break;
		case eGizmoEditType::ROTATE: m_controlNode->m_transform.rot *= transform.rot; break;
		}

		m_transform.pos += transform.pos;
	}
	else // WORLD
	{
		Matrix44 worldToLocal;
		if (m_controlNode->m_parent)
			worldToLocal = m_controlNode->m_parent->GetWorldMatrix().Inverse();
		const Matrix44 localTm = m_controlNode->GetWorldMatrix() * transform.GetMatrix() * worldToLocal;

		switch (m_type)
		{
		case eGizmoEditType::TRANSLATE: m_controlNode->m_transform.pos = localTm.GetPosition(); break;
		//case eGizmoEditType::SCALE: m_controlNode->m_transform.scale = localTm.GetScale(); break; bug occur (no rogical bug)
		case eGizmoEditType::ROTATE: m_controlNode->m_transform.rot = localTm.GetQuaternion(); break;
		}

		const Matrix44 worldTm = m_controlNode->GetWorldMatrix();
		m_transform.pos = worldTm.GetPosition();
	}

	// Update Tile Allocation
	RET(!m_controlNode->m_parent);
	RET(m_controlNode->m_parent->m_type != eNodeType::TERRAIN);
	RET(eGizmoEditType::TRANSLATE != m_type);
	{
		//cBoundingBox bbox = m_controlNode->m_boundingBox;
		//const Transform transform = m_controlNode->GetWorldTransform();
		//bbox *= transform.GetMatrix();
		// insert model to most nearest tile
		//{
		//	vector<cTile*> candidate;
		//	for (auto &tile : g_root.m_terrain.m_tiles)
		//	{
		//		cBoundingBox tbbox = tile->m_boundingBox * tile->GetWorldMatrix();
		//		if (tbbox.Collision(bbox))
		//			candidate.push_back(tile);
		//	}

		//	assert(!candidate.empty());

		//	float nearLen = FLT_MAX;
		//	cTile *nearTile = NULL;
		//	for (auto &tile : candidate)
		//	{
		//		const float len = tile->GetWorldMatrix().GetPosition().Distance(transform.pos);
		//		if (len < nearLen)
		//		{
		//			nearLen = len;
		//			nearTile = tile;
		//		}
		//	}

		//	m_controlNode->m_transform = transform * nearTile->m_transform.Inverse();
		//	m_controlNode->CalcBoundingSphere();
		//	m_controlNode->m_parent->RemoveChild(m_controlNode, false);
		//	nearTile->AddChild(m_controlNode);
		//}
	}
}


void cGizmo::Cancel()
{
	m_controlNode = NULL;
}


// return true if Gizmo Edit mode
// out : Gizmo Edit type
bool cGizmo::Render(cRenderer &renderer
	, const float deltaSeconds
	, const POINT &mousePos
	, const bool isMouseClick
	, const XMMATRIX &parentTm //= XMIdentity
)
{
	RETV(!m_controlNode, false);

	m_mousePos = mousePos;

	switch (m_type)
	{
	case eGizmoEditType::TRANSLATE: RenderTranslate(renderer, parentTm);break;
	case eGizmoEditType::SCALE: RenderScale(renderer, parentTm); break;
	case eGizmoEditType::ROTATE: RenderRotate(renderer, parentTm); break;
	default: assert(0); break;
	}

	if (isMouseClick)
	{
		for (int i=0; i < 6; ++i)
		{
			if (m_pick[i])
			{
				m_isKeepEdit = true;
				m_axisType = (eGizmoEditAxis::Enum)i;
				m_prevMousePos = mousePos;
				break;
			}
		}
	}
	else
	{
		m_isKeepEdit = false;
	}

	return m_isKeepEdit;
}


void cGizmo::RenderTranslate(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
)
{
	ZeroMemory(m_pick, sizeof(m_pick));

	if (m_isKeepEdit)
		m_pick[m_axisType] = true;

	const Ray ray = GetMainCamera().GetRay(m_mousePos.x, m_mousePos.y);
	const float arrowSize = 0.17f;

	{
		// fixed gizmo position form Camera Eye Pos
		const Matrix44 worldTm = m_controlNode->GetWorldMatrix();
		Vector3 v = worldTm.GetPosition() - GetMainCamera().GetEyePos();
		m_transform.pos = v.Normal() * 3 + GetMainCamera().GetEyePos();
	}

	const float scale = 0.1f;
	Matrix44 scaleTm;
	scaleTm.SetScale(scale);
	const XMMATRIX ptm = scaleTm.GetMatrixXM() * parentTm;

	Transform boundingBoxScale;
	boundingBoxScale.scale = Vector3(1, 1, 1)*0.8f;
	const XMMATRIX mBScale = boundingBoxScale.GetMatrixXM();

	float dist[6];
	Transform tfm = m_transform;
	tfm.pos *= 1 / scale;
	const Matrix44 m = tfm.GetMatrix();

	// X-Axis
	const Vector3 px0 = Vector3(0.3f, 0, 0) * m;
	const Vector3 px1 = Vector3(2, 0, 0) * m;
	m_arrow[0].SetDirection(px0, px1, arrowSize);
	if (!m_isKeepEdit)
		m_pick[0] = m_arrow[0].Picking(ray, ptm, false, &dist[0]);

	// Y-Axis
	const Vector3 py0 = Vector3(0, 0.3f, 0) * m;
	const Vector3 py1 = Vector3(0, 2, 0) * m;
	m_arrow[1].SetDirection(py0, py1, arrowSize);
	if (!m_isKeepEdit)
		m_pick[1] = m_arrow[1].Picking(ray, ptm, false, &dist[1]);

	// Z-Axis
	const Vector3 pz0 = Vector3(0, 0, 0.3f) * m;
	const Vector3 pz1 = Vector3(0, 0, 2) * m;
	m_arrow[2].SetDirection(pz0, pz1, arrowSize);
	if (!m_isKeepEdit)
		m_pick[2] = m_arrow[2].Picking(ray, ptm, false, &dist[2]);

	XMMATRIX planeTm[3];// X-Z, Y-Z, X-Y Plane
	{
		const XMMATRIX tm = tfm.GetMatrixXM() * ptm;

		// X-Z Plane
		Transform tfmXZ;
		tfmXZ.rot.SetRotationArc(Vector3(0, 1, 0), Vector3(0, 0, 1));
		tfmXZ.pos = Vector3(2.f, 0, 2.f);
		const XMMATRIX ptmXZ = tfmXZ.GetMatrixXM() * tm;
		if (!m_isKeepEdit)
			m_pick[3] = m_quad.Picking(ray, eNodeType::MODEL, ptmXZ, false, &dist[3]) ? true : false;
		planeTm[0] = ptmXZ;

		// Y-Z Plane
		Transform tfmYZ;
		tfmYZ.rot.SetRotationArc(Vector3(1, 0, 0), Vector3(0, 0, 1));
		tfmYZ.pos = Vector3(0, 2.f, 2.f);
		const XMMATRIX ptmYZ = tfmYZ.GetMatrixXM() * tm;
		if (!m_isKeepEdit)
			m_pick[4] = m_quad.Picking(ray, eNodeType::MODEL, ptmYZ, false, &dist[4]) ? true : false;
		planeTm[1] = ptmYZ;

		// X-Y Plane
		Transform tfmXY;
		tfmXY.pos = Vector3(2.f, 2.f, 0);
		const XMMATRIX ptmXY = tfmXY.GetMatrixXM() * tm;
		if (!m_isKeepEdit)
			m_pick[5] = m_quad.Picking(ray, eNodeType::MODEL, ptmXY, false, &dist[5]) ? true : false;
		planeTm[2] = ptmXY;
	}

	int pickCount = 0;
	for (int i = 0; i < 6; ++i)
		if (m_pick[i])
			++pickCount;

	// Find Best Pick
	if (pickCount > 1)
	{
		Vector3 axisPos[6]; // 기즈모의 각 축의 위치

		Transform tmp = m_arrow[0].m_transform.GetMatrixXM() * ptm;
		axisPos[0] = tmp.pos;
		tmp = m_arrow[1].m_transform.GetMatrixXM() * ptm;
		axisPos[1] = tmp.pos;
		tmp = m_arrow[2].m_transform.GetMatrixXM() * ptm;
		axisPos[2] = tmp.pos;
		tmp = m_quad.m_transform.GetMatrixXM() * planeTm[0];
		axisPos[3] = tmp.pos;
		tmp = m_quad.m_transform.GetMatrixXM() * planeTm[1];
		axisPos[4] = tmp.pos;
		tmp = m_quad.m_transform.GetMatrixXM() * planeTm[2];
		axisPos[5] = tmp.pos;

		float nearLen = FLT_MAX;
		int idx = -1;
		for (int i = 0; i < 6; ++i)
		{
			if (m_pick[i])
			{
				if (nearLen > dist[i])
				{
					nearLen = dist[i];
					idx = i;
				}
			}
		}

		if (idx >= 0)
		{
			ZeroMemory(m_pick, sizeof(m_pick));
			m_pick[idx] = true;			
		}
	}

	m_arrow[0].m_color = m_pick[0] ? cColor::YELLOW : cColor::RED;
	m_arrow[0].Render(renderer, ptm);
	m_arrow[1].m_color = m_pick[1] ? cColor::YELLOW : cColor::GREEN;
	m_arrow[1].Render(renderer, ptm);
	m_arrow[2].m_color = m_pick[2] ? cColor::YELLOW : cColor::BLUE;
	m_arrow[2].Render(renderer, ptm);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(states.CullNone());
	{
		// X-Z Plane
		m_quad.m_color = m_pick[3] ? cColor(0.8f, 0.8f, 0, 0.8f) : cColor(0.6f, 0.6f, 0, 0.6f);
		m_quad.Render(renderer, planeTm[0]);
		// Y-Z Plane
		m_quad.m_color = m_pick[4] ? cColor(0.8f, 0.8f, 0, 0.8f) : cColor(0.6f, 0.6f, 0, 0.6f);
		m_quad.Render(renderer, planeTm[1]);
		// X-Y Plane
		m_quad.m_color = m_pick[5] ? cColor(0.8f, 0.8f, 0, 0.8f) : cColor(0.6f, 0.6f, 0, 0.6f);
		m_quad.Render(renderer, planeTm[2]);
	}
	renderer.GetDevContext()->RSSetState(states.CullCounterClockwise());

	// recovery material
	m_quad.m_color = cColor::WHITE;


	if (m_isKeepEdit)
	{
		const Vector3 nodePosW = m_controlNode->GetWorldMatrix().GetPosition();
		Plane groundXZ(Vector3(0, 1, 0), nodePosW);
		Plane groundYZ(Vector3(1, 0, 0), nodePosW);
		Plane groundXY(Vector3(0, 0, 1), nodePosW);
		const Vector3 curPosXZ = groundXZ.Pick(ray.orig, ray.dir);
		const Vector3 curPosYZ = groundYZ.Pick(ray.orig, ray.dir);
		const Vector3 curPosXY = groundXY.Pick(ray.orig, ray.dir);
		const Ray prevRay = GetMainCamera().GetRay(m_prevMousePos.x, m_prevMousePos.y);
		const Vector3 prevPosXZ = groundXZ.Pick(prevRay.orig, prevRay.dir);
		const Vector3 prevPosYZ = groundYZ.Pick(prevRay.orig, prevRay.dir);
		const Vector3 prevPosXY = groundXY.Pick(prevRay.orig, prevRay.dir);
		m_prevMousePos = m_mousePos;

		// Translate Edit
		Transform change;
		switch (m_axisType)
		{
		case eGizmoEditAxis::X:
			change.pos.x = (curPosXZ.x - prevPosXZ.x);
			break;

		case eGizmoEditAxis::Y:
			change.pos.y = (curPosYZ.y - prevPosYZ.y);
			break;

		case eGizmoEditAxis::Z:
			change.pos.z = (curPosXZ.z - prevPosXZ.z);
			break;

		case eGizmoEditAxis::XZ:
			change.pos.x = (curPosXZ.x - prevPosXZ.x);
			change.pos.z = (curPosXZ.z - prevPosXZ.z);
			break;

		case eGizmoEditAxis::YZ:
			change.pos.y = (curPosYZ.y - prevPosYZ.y);
			change.pos.z = (curPosYZ.z - prevPosYZ.z);
			break;

		case eGizmoEditAxis::XY:
			change.pos.x = (curPosXY.x - prevPosXY.x);
			change.pos.y = (curPosXY.y - prevPosXY.y);
			break;

		default: assert(0); break;
		}

		UpdateNodeTransform(change);
	}
}


void cGizmo::RenderScale(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
)
{
	ZeroMemory(m_pick, sizeof(m_pick));
	if (m_isKeepEdit)
		m_pick[m_axisType] = true;

	const Ray ray = GetMainCamera().GetRay(m_mousePos.x, m_mousePos.y);
	const float arrowSize = 0.17f;

	{
		// fixed gizmo position form Camera Eye Pos
		const Matrix44 worldTm = m_controlNode->GetWorldMatrix();
		Vector3 v = worldTm.GetPosition() - GetMainCamera().GetEyePos();
		m_transform.pos = v.Normal() * 3 + GetMainCamera().GetEyePos();
	}

	const float scale = 0.1f;
	Matrix44 scaleTm;
	scaleTm.SetScale(scale);
	const XMMATRIX ptm = scaleTm.GetMatrixXM() * parentTm;

	Transform tfm = m_transform;
	tfm.pos *= 1 / scale;
	const Matrix44 m = tfm.GetMatrix();
	const Vector3 px0 = Vector3(0.3f, 0, 0) * m;
	const Vector3 px1 = Vector3(2, 0, 0) * m;
	m_arrow[0].SetDirection(px0, px1, arrowSize);
	if (!m_isKeepEdit)
		m_pick[0] = m_arrow[0].Picking(ray, ptm, false);
	m_arrow[0].m_color = m_pick[0] ? cColor::YELLOW : cColor::RED;
	m_arrow[0].Render(renderer, ptm);

	const Vector3 py0 = Vector3(0, 0.3f, 0) * m;
	const Vector3 py1 = Vector3(0, 2, 0) * m;
	m_arrow[1].SetDirection(py0, py1, arrowSize);
	if (!m_isKeepEdit)
		m_pick[1] = m_arrow[1].Picking(ray, ptm, false);
	m_arrow[1].m_color = m_pick[1] ? cColor::YELLOW : cColor::GREEN;
	m_arrow[1].Render(renderer, ptm);

	const Vector3 pz0 = Vector3(0, 0, 0.3f) * m;
	const Vector3 pz1 = Vector3(0, 0, 2) * m;
	m_arrow[2].SetDirection(pz0, pz1, arrowSize);
	if (!m_isKeepEdit)
		m_pick[2] = m_arrow[2].Picking(ray, ptm, false);
	m_arrow[2].m_color = m_pick[2] ? cColor::YELLOW : cColor::BLUE;
	m_arrow[2].Render(renderer, ptm);

	// recovery material
	m_quad.m_color = cColor::WHITE;

	if (m_isKeepEdit)
	{
		const Vector3 nodePosW = m_controlNode->GetWorldMatrix().GetPosition();
		Plane groundXZ(Vector3(0, 1, 0), nodePosW);
		Plane groundYZ(Vector3(1, 0, 0), nodePosW);
		//Plane groundXY(Vector3(0, 0, 1), m_transform.pos);
		const Vector3 curPosXZ = groundXZ.Pick(ray.orig, ray.dir);
		const Vector3 curPosYZ = groundYZ.Pick(ray.orig, ray.dir);
		//const Vector3 curPosXY = groundXY.Pick(ray.orig, ray.dir);
		const Ray prevRay = GetMainCamera().GetRay(m_prevMousePos.x, m_prevMousePos.y);
		const Vector3 prevPosXZ = groundXZ.Pick(prevRay.orig, prevRay.dir);
		const Vector3 prevPosYZ = groundYZ.Pick(prevRay.orig, prevRay.dir);
		//const Vector3 prevPosXY = groundXY.Pick(prevRay.orig, prevRay.dir);
		m_prevMousePos = m_mousePos;

		// Scale Edit
		Transform change;
		switch (m_axisType)
		{
		case eGizmoEditAxis::X:
			change.scale.x += (curPosXZ.x - prevPosXZ.x) * 0.1f;
			break;
		case eGizmoEditAxis::Y:
			change.scale.y += (curPosYZ.y - prevPosYZ.y) * 0.1f;
			break;
		case eGizmoEditAxis::Z:
			change.scale.z += (curPosXZ.z - prevPosXZ.z) * 0.1f;
			break;
		default: assert(0); break;
		}

		UpdateNodeTransform(change);
	}
}


void cGizmo::RenderRotate(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
)
{
	ZeroMemory(m_pick, sizeof(m_pick));
	if (m_isKeepEdit)
		m_pick[m_axisType] = true;

	const Ray ray = GetMainCamera().GetRay(m_mousePos.x, m_mousePos.y);

	{
		// fixed gizmo position form Camera Eye Pos
		const Matrix44 worldTm = m_controlNode->GetWorldMatrix();
		Vector3 v = worldTm.GetPosition() - GetMainCamera().GetEyePos();
		m_transform.pos = v.Normal() * 3 + GetMainCamera().GetEyePos();
	}

	const float scale = 0.1f;
	Matrix44 scaleTm;
	scaleTm.SetScale(scale);

	Matrix44 invScaleTm;
	invScaleTm.SetScale(1.f / scale);

	struct sData {
		cBoundingBox bbox;
		float distance;
		int axis; //0=x, 1=y, 2=z
	};
	sData pickes[8]; // for sorting
	int pickesCnt = 0;


	// X-Axis
	Transform tfmX = m_transform;
	tfmX.rot.SetRotationArc(Vector3(1, 0, 0), Vector3(0, 1, 0));
	tfmX.pos *= 1 / scale;
	const XMMATRIX tmX = tfmX.GetMatrixXM() * scaleTm.GetMatrixXM() * parentTm;
	if (!m_isKeepEdit)
	{
		for (auto &bbox : m_ringBbox)
		{
			cBoundingBox box = bbox;
			box *= tmX;
			float dist = 0;
			if (box.Pick(ray, &dist))
			{
				if (pickesCnt < ARRAYSIZE(pickes))
					pickes[pickesCnt++] = { box, dist, 0 };
			}
		}
	}


	// Y-Axis
	Transform tfmY = m_transform;
	tfmY.pos *= 1 / scale;
	const XMMATRIX tmY = tfmY.GetMatrixXM() * scaleTm.GetMatrixXM() * parentTm;
	if (!m_isKeepEdit)
	{
		for (auto &bbox : m_ringBbox)
		{
			cBoundingBox box = bbox;
			box *= tmY;
			float dist = 0;
			if (box.Pick(ray, &dist))
			{
				if (pickesCnt < ARRAYSIZE(pickes))
					pickes[pickesCnt++] = { box, dist, 1 };
			}
		}
	}


	// Z-Axis
	Transform tfmZ = m_transform;
	tfmZ.rot.SetRotationArc(Vector3(0, 0, 1), Vector3(0, 1, 0));
	tfmZ.pos *= 1 / scale;
	const XMMATRIX tmZ = tfmZ.GetMatrixXM() * scaleTm.GetMatrixXM() * parentTm;
	if (!m_isKeepEdit)
	{
		for (auto &bbox : m_ringBbox)
		{
			cBoundingBox box = bbox;
			box *= tmZ;
			float dist = 0;
			if (box.Pick(ray, &dist))
			{
				if (pickesCnt < ARRAYSIZE(pickes))
					pickes[pickesCnt++] = { box, dist, 2 };
			}
		}
	}

	if (pickesCnt >= 1)
	{
		int axis = pickes[0].axis;
		float mostNearestBox = pickes[0].distance;
		for (int i = 1; i < pickesCnt; ++i)
		{
			if (mostNearestBox > pickes[i].distance)
			{
				axis = pickes[i].axis;
				mostNearestBox = pickes[i].distance;
			}
		}

		m_pick[axis] = true;
	}

	if (m_isKeepEdit)
	{
		Plane groundXZ(Vector3(0, 1, 0), m_transform.pos);
		Plane groundYZ(Vector3(1, 0, 0), m_transform.pos);
		Plane groundXY(Vector3(0, 0, 1), m_transform.pos);
		const Vector3 curPosXZ = groundXZ.Pick(ray.orig, ray.dir);
		const Vector3 curPosYZ = groundYZ.Pick(ray.orig, ray.dir);
		const Vector3 curPosXY = groundXY.Pick(ray.orig, ray.dir);
		const Ray prevRay = GetMainCamera().GetRay(m_prevMousePos.x, m_prevMousePos.y);
		const Vector3 prevPosXZ = groundXZ.Pick(prevRay.orig, prevRay.dir);
		const Vector3 prevPosYZ = groundYZ.Pick(prevRay.orig, prevRay.dir);
		const Vector3 prevPosXY = groundXY.Pick(prevRay.orig, prevRay.dir);
		m_prevMousePos = m_mousePos;

		Transform change;
		switch (m_axisType)
		{
		case eGizmoEditAxis::X:
		{
			Vector3 v1 = (curPosYZ - m_transform.pos).Normal();
			const Vector3 p0 = m_transform.pos * invScaleTm;
			const Vector3 p1 = (v1*1.f*scale + m_transform.pos) * invScaleTm;
			m_arrow[0].SetDirection(p0, p1, 0.15f);
			m_arrow[0].m_color = cColor::YELLOW;
			m_arrow[0].Render(renderer, scaleTm.GetMatrixXM() * parentTm);

			m_torus.m_color = m_pick[0] ? cColor::YELLOW : cColor(0.9f, 0, 0, 1);
			m_torus.Render(renderer, tmX);

			Vector3 v0 = (prevPosYZ - m_transform.pos).Normal();
			change.rot.SetRotationArc(v0, v1);
		}
		break;

		case eGizmoEditAxis::Y:
		{
			Vector3 v1 = (curPosXZ - m_transform.pos).Normal();
			const Vector3 p0 = m_transform.pos * invScaleTm;
			const Vector3 p1 = (v1*1.f*scale + m_transform.pos) * invScaleTm;
			m_arrow[0].SetDirection(p0, p1, 0.15f);
			m_arrow[0].m_color = cColor::YELLOW;
			m_arrow[0].Render(renderer, scaleTm.GetMatrixXM() * parentTm);

			m_torus.m_color = m_pick[1] ? cColor::YELLOW : cColor(0, 0.9f, 0, 1);
			m_torus.Render(renderer, tmY);

			Vector3 v0 = (prevPosXZ - m_transform.pos).Normal();
			change.rot.SetRotationArc(v0, v1);
		}
		break;

		case eGizmoEditAxis::Z:
		{
			Vector3 v1 = (curPosXY - m_transform.pos).Normal();
			const Vector3 p0 = m_transform.pos * invScaleTm;
			const Vector3 p1 = (v1*1.f*scale + m_transform.pos) * invScaleTm;
			m_arrow[0].SetDirection(p0, p1, 0.15f);
			m_arrow[0].m_color = cColor::YELLOW;
			m_arrow[0].Render(renderer, scaleTm.GetMatrixXM() * parentTm);

			m_torus.m_color = m_pick[2] ? cColor::YELLOW : cColor(0, 0, 0.9f, 1);
			m_torus.Render(renderer, tmZ);

			Vector3 v0 = (prevPosXY - m_transform.pos).Normal();
			change.rot.SetRotationArc(v0, v1);
		}
		break;

		default: assert(0); break;
		}

		UpdateNodeTransform(change);
	}
	else
	{
		m_torus.m_color = m_pick[0] ? cColor::YELLOW : cColor(0.9f, 0, 0, 1);
		m_torus.Render(renderer, tmX);
		m_torus.m_color = m_pick[1] ? cColor::YELLOW : cColor(0, 0.9f, 0, 1);
		m_torus.Render(renderer, tmY);
		m_torus.m_color = m_pick[2] ? cColor::YELLOW : cColor(0, 0, 0.9f, 1);
		m_torus.Render(renderer, tmZ);
	}

	// recovery material
	m_quad.m_color = cColor::WHITE;
}
