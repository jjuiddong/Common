//
// 2017-10-02, jjuiddong
// Gizmo
//
#pragma once


namespace graphic
{

	struct eGizmoEditType {
		enum Enum {TRANSLATE, ROTATE, SCALE};
	};

	struct eGizmoEditAxis {
		enum Enum {X, Y, Z, XZ, YZ, XY};
	};

	struct eGizmoTransform {
		enum Enum { LOCAL, WORLD};
	};


	class cGizmo : public cNode2
	{
	public:
		cGizmo();
		virtual ~cGizmo();

		bool Create(cRenderer &renderer);
		void SetControlNode(cNode2 *node);
		void SetTransformType(const eGizmoTransform::Enum type);

		bool Render(cRenderer &renderer
			, const float deltaSeconds
			, const POINT &mousePos
			, const bool isMouseClick
			, const XMMATRIX &parentTm = XMIdentity
		);

		bool IsEditMode() const;
		void Cancel();


	protected:
		void RenderTranslate(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity);
		void RenderScale(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity);
		void RenderRotate(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity);
		void UpdateNodeTransform(const Transform &transform);


	public:
		bool m_isEdit; // if picking node? true
		eGizmoEditType::Enum m_type;
		eGizmoEditAxis::Enum m_axisType;
		eGizmoTransform::Enum m_transformType;
		cNode2 *m_controlNode;
		cQuad m_quad;
		cTorus m_torus;
		cDbgArrow m_arrow;
		bool m_pick[6]; // x,y,z-axis, x-z, y-z, x-y plane (eGizmoEditAxis order)
		float m_deltaSconds;
		POINT m_prevMousePos;
		POINT m_mousePos;
		vector<cBoundingBox> m_ringBbox; // rotation ring picking
	};


	inline bool cGizmo::IsEditMode() const { return m_controlNode && m_isEdit; }
}
