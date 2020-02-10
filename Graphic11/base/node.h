//
// 2017-07-11, jjuiddong
// 화면에 출력되는 모든 객체는 이 클래스로부터 상속받는다.
//
#pragma once


namespace graphic
{
	using namespace common;

	class cRenderer;
	class cFrustum;
	class cShader11;

	struct eNodeType {
		enum Enum { NONE, MODEL, TEXT, TERRAIN, VIRTUAL };
	};

	DECLARE_ENUM(eSubType, NONE, CUBE2, AREA, CUBE, LINE, RACK);


	struct eRenderFlag {
		enum Enum {
			NONE = 1 << 0
			, VISIBLE = NONE
			, TERRAIN = 1 << 1
			, MODEL = 1 << 2
			, ALPHABLEND = 1 << 3
			, NOALPHABLEND = 1 << 4
			, SHADOW = 1 << 5
			, OUTLINE = 1 << 6
			, TEXT = 1 << 7
			, NODEPTH = 1 << 8
			, WIREFRAME = 1 << 9
		};
	};

	struct eOpFlag {
		enum Enum {
			NONE = 0
			, COLLISION = 1 << 0
			, PICK = 1 << 1
		};
	};

	struct ePickState{
		enum Enum {
			HOVER,
			CLICK,
			DBCLICK,
		};
	};


	class cNode
	{
	public:
		cNode();
		cNode(const int id, const StrId &name = "none"
			, const eNodeType::Enum type = eNodeType::MODEL
			, const eSubType::Enum subType = eSubType::NONE);
		virtual ~cNode();

		virtual bool Update(cRenderer &renderer, const float deltaSeconds);
		
		virtual bool Render(cRenderer &renderer
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1);

		virtual bool RenderInstancing(cRenderer &renderer
			, const int count
			, const Matrix44 *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1
		);

		virtual bool RenderTessellation(cRenderer &renderer
			, const int controlPointCount
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1);


		virtual bool AddChild(cNode *node);
		virtual cNode* FindNode(const int id);
		virtual cNode* FindNode(const StrId &name);
		virtual void FindNodeAll(const StrId &name, OUT vector<cNode*> &out);
		virtual bool RemoveChild(const int id, const bool rmInstance=true);
		virtual bool RemoveChild(cNode *rmNode, const bool rmInstance = true);
		
		virtual void SetShader(cShader11 *shader) { m_shader = shader; }
		virtual void CalcBoundingSphere();
		virtual float CullingTest(const cFrustum &frustum
			, const XMMATRIX &tm = XMIdentity
			, const bool isModel = true);
		
		virtual cNode* Picking(const Ray &ray, const eNodeType::Enum type
			, const XMMATRIX &parentTm, const bool isSpherePicking = true
			, OUT float *dist =NULL);

		virtual cNode* Picking(const Ray &ray, const eNodeType::Enum type
			, const bool isSpherePicking = true
			, OUT float *dist = NULL);

		virtual void Picking(const Ray &ray, const eNodeType::Enum type
			, const XMMATRIX &parentTm, const bool isSpherePicking
			, OUT vector<std::pair<cNode*, float>> &out);

		virtual cNode* Clone(cRenderer &renderer) const { return NULL; }
		virtual void Clear();

		Matrix44 GetWorldMatrix() const;
		Matrix44 GetParentWorldMatrix() const;
		Transform GetWorldTransform() const;
		virtual void SetTechnique(const char *techniqName, const bool isApplyChild=true);

		inline bool IsVisible() const;
		inline int SetRenderFlag(const eRenderFlag::Enum val, const bool enable);
		inline bool IsRenderFlag(const eRenderFlag::Enum val);
		inline void ClearRenderFlag();
		inline int SetOpFlag(const eOpFlag::Enum val, const bool enable);
		inline bool IsOpFlag(const eOpFlag::Enum val);
		inline void ClearOpFlag();

		// Picking Event, call from PickManager
		virtual void OnPicking(const ePickState::Enum state) {}


	public:
		int m_id; // unique id
		StrId m_name;
		bool m_isEnable; // if false, didn't show
		int m_renderFlags; // Composition eRenderFlag, default : VISIBLE
		int m_opFlags; // Composition eOpFlag, defalut : NONE
		int m_vtxType; // Composition eVertexType
		eNodeType::Enum m_type;
		eSubType::Enum m_subType;
		cNode *m_parent;
		vector<cNode*> m_children;
		Transform m_localTm; // tricky code (solution: change mesh position, orientation)
		Transform m_transform;
		cShader11 *m_shader;
		Str32 m_techniqueName;
		cBoundingBox m_boundingBox; // Local Space
		cBoundingSphere m_boundingSphere; // Local Space
		float m_alphaRadius; // simple bounding sphere radius (calculate in CalcBoundingSphere)
		Vector3 m_alphaNormal; // for plane normal vector for alpha blending sorting (optimize)
							   // default: Vector3(0,1,0)
	};


	inline bool cNode::IsVisible() const { return (m_renderFlags & eRenderFlag::VISIBLE); }
	inline int cNode::SetRenderFlag(const eRenderFlag::Enum val, const bool enable) {
		return (enable) ? (m_renderFlags |= val) : (m_renderFlags &= ~val);
	}
	inline bool cNode::IsRenderFlag(const eRenderFlag::Enum val) {
		return (m_renderFlags & val) ? true : false;
	}
	inline int cNode::SetOpFlag(const eOpFlag::Enum val, const bool enable) {
		return (enable) ? (m_opFlags |= val) : (m_opFlags &= ~val);
	}
	inline bool cNode::IsOpFlag(const eOpFlag::Enum val) {
		return (m_opFlags & val)? true : false;
	}
	inline void cNode::ClearRenderFlag() {m_renderFlags = 0;}
	inline void cNode::ClearOpFlag(){m_opFlags = 0;}

}
