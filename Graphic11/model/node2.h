//
// 2017-07-11, jjuiddong
// 화면에 출력되는 모든 객체는 이 클래스로부터 상속받는다.
//
#pragma once


namespace graphic
{
	class cRenderer;
	class cFrustum;

	struct eNodeType {
		enum Enum { NONE, BONE, MESH, MODEL, TEXT, TERRAIN };
	};

	struct eRenderFlag {
		enum Enum {
			NONE = 0x01
			, BUILDING = 0x02
			, ALPHABLEND = 0x4
			, SHADOW = 0x08
		};
	};


	class cNode2
	{
	public:
		cNode2();
		cNode2(const int id, const StrId &name = "none", const eNodeType::Enum type=eNodeType::NONE);
		virtual ~cNode2();

		virtual bool Update(cRenderer &renderer, const float deltaSeconds);
		virtual bool Render(cRenderer &renderer, const Matrix44 &parentTm = Matrix44::Identity, const int flags = 1);
		virtual bool AddChild(cNode2 *node);
		virtual const cNode2* FindNode(const int id) const;
		virtual const cNode2* FindNode(const StrId &name) const;
		virtual bool RemoveChild(const int id, const bool rmInstance=true);
		virtual bool RemoveChild(cNode2 *rmNode, const bool rmInstance = true);
		virtual void SetShader(cShader *shader) { m_shader = shader; }
		virtual void CalcBoundingSphere();
		virtual float CullingTest(const cFrustum &frustum, const Matrix44 &tm = Matrix44::Identity, const bool isModel = true);
		virtual cNode2* Picking(const Vector3 &orig, const Vector3 &dir, const eNodeType::Enum type);
		virtual void LostDevice() {}
		virtual void ResetDevice(cRenderer &renderer) {}
		virtual void Clear();
		Matrix44 GetWorldMatrix() const;


	public:
		int m_id;
		StrId m_name;
		bool m_isEnable; // if false, didn't show
		bool m_isShow;
		bool m_isShadowEnable;
		bool m_isShadow;
		int m_flags; // eRenderFlag, default : VISIBLE
		eNodeType::Enum m_type;
		cNode2 *m_parent;
		vector<cNode2*> m_children;
		Transform m_transform;
		cShader *m_shader;
		cBoundingBox m_boundingBox; // Local Space
		cBoundingSphere m_boundingSphere; // Local Space
	};

}
