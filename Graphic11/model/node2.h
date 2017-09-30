//
// 2017-07-11, jjuiddong
// 화면에 출력되는 모든 객체는 이 클래스로부터 상속받는다.
//
#pragma once


namespace graphic
{

	struct eNodeType {
		enum Enum { NONE, BONE, MESH, MODEL, TEXT, TERRAIN };
	};

	struct eRenderFlag {
		enum Enum {
			NONE = 1 << 0
			, VISIBLE = NONE
			, BUILDING = 1 << 1
			, ALPHABLEND = 1 << 2
			, SHADOW = 1 << 3
		};
	};

	struct eOpFlag {
		enum Enum {
			NONE = 0
			, COLLISION = 1 << 0
		};
	};


	class cRenderer;
	class cFrustum;
	class cShader11;

	class cNode2
	{
	public:
		cNode2();
		cNode2(const int id, const StrId &name = "none", const eNodeType::Enum type=eNodeType::NONE);
		virtual ~cNode2();

		virtual bool Update(cRenderer &renderer, const float deltaSeconds);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1);
		virtual bool RenderInstancing(cRenderer &renderer
			, const int count
			, const XMMATRIX *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1
		);

		virtual bool AddChild(cNode2 *node);
		virtual const cNode2* FindNode(const int id) const;
		virtual const cNode2* FindNode(const StrId &name) const;
		virtual bool RemoveChild(const int id, const bool rmInstance=true);
		virtual bool RemoveChild(cNode2 *rmNode, const bool rmInstance = true);
		
		virtual void SetShader(cShader11 *shader) { m_shader = shader; }
		virtual void CalcBoundingSphere();
		virtual float CullingTest(const cFrustum &frustum, const XMMATRIX &tm = XMIdentity, const bool isModel = true);
		virtual cNode2* Picking(const Vector3 &orig, const Vector3 &dir, const eNodeType::Enum type);
		virtual void LostDevice() {}
		virtual void ResetDevice(cRenderer &renderer) {}
		virtual void Clear();

		Matrix44 GetWorldMatrix() const;
		Transform GetWorldTransform() const;
		void SetTechnique(const char *techniqName, const bool isApplyChild=true);

		inline bool IsVisible() const;
		inline int SetRenderFlag(const eRenderFlag::Enum val, const bool enable);
		inline void ClearRenderFlag();
		inline int SetOpFlag(const eOpFlag::Enum val, const bool enable);
		inline void ClearOpFlag();


	public:
		int m_id; // unique id
		StrId m_name;
		bool m_isEnable; // if false, didn't show
		int m_renderFlags; // eRenderFlag, default : VISIBLE
		int m_opFlags; // eOpFlag, defalut : NONE
		eNodeType::Enum m_type;
		cNode2 *m_parent;
		vector<cNode2*> m_children;
		Transform m_transform;
		cShader11 *m_shader;
		Str32 m_techniqueName;
		cBoundingBox m_boundingBox; // Local Space
		cBoundingSphere m_boundingSphere; // Local Space
	};


	inline bool cNode2::IsVisible() const { return (m_renderFlags & eRenderFlag::VISIBLE); }
	inline int cNode2::SetRenderFlag(const eRenderFlag::Enum val, const bool enable) {
		return (enable) ? (m_renderFlags |= val) : (m_renderFlags &= ~val);
	}
	inline int cNode2::SetOpFlag(const eOpFlag::Enum val, const bool enable) {
		return (enable) ? (m_opFlags |= val) : (m_opFlags &= ~val);
	}
	inline void cNode2::ClearRenderFlag() {m_renderFlags = 0;}
	inline void cNode2::ClearOpFlag(){m_opFlags = 0;}

}
