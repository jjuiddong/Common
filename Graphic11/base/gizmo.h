//
// 2017-10-02, jjuiddong
// Gizmo
//
#pragma once


namespace graphic
{
	using namespace common;

	struct eGizmoEditType {
		enum Enum {TRANSLATE, ROTATE, SCALE, None};
	};

	struct eGizmoEditAxis {
		enum Enum {X, Y, Z, XZ, YZ, XY};
	};

	struct eGizmoTransform {
		enum Enum { LOCAL, WORLD};
	};


	class cGizmo : public cNode
	{
	public:
		cGizmo();
		virtual ~cGizmo();

		bool Create(cRenderer &renderer, const bool isImmediate = true);
		void SetControlNode(cNode *node);
		void SetTransformType(const eGizmoTransform::Enum type);

		bool Render(cRenderer &renderer
			, const float deltaSeconds
			, const POINT &mousePos
			, const bool isMouseDown
			, const XMMATRIX &parentTm = XMIdentity
		);

		void LockEditType(const eGizmoEditType::Enum type, const bool lock);
		void UpdateTargetTransform(const Transform &transform);
		bool IsKeepEditMode() const;
		void Cancel();


	protected:
		void RenderTranslate(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity);
		void RenderScale(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity);
		void RenderRotate(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity);
		void UpdateNodeTransform(const Transform &transform);


	public:
		bool m_isKeepEdit; // if picking node? true
		bool m_isImmediate; // immediate update node transform?
		eGizmoEditType::Enum m_type;
		eGizmoEditAxis::Enum m_axisType;
		eGizmoTransform::Enum m_transformType;
		cNode *m_controlNode; // reference
		cQuad m_quad;
		cTorus m_torus;
		cDbgArrow m_arrow[3]; // x,y,z axis
		bool m_pick[6]; // x,y,z-axis, x-z, y-z, x-y plane (eGizmoEditAxis order)
		bool m_lock[4]; // lock edit type (eGizmoEditType index)
		POINT m_prevMousePos;
		POINT m_mousePos; // window 2d mouse pos
		bool m_prevMouseDown; //check clicked mouse left button
		vector<cBoundingBox> m_ringBbox; // rotation ring picking
		Transform m_targetTransform; // temporal store node transform (to multithread access)
		Transform m_deltaTransform; // gizmo delta transform
	};


	inline bool cGizmo::IsKeepEditMode() const { return m_controlNode && m_isKeepEdit; }
}
