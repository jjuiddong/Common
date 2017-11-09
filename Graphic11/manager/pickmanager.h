//
// 2017-03-14, jjuiddong
// Picking Check, Notify Event
//
// Refactoring, 2017-10-15
//
#pragma once


namespace graphic
{

	interface iPickListener {
		virtual void OnPickEvent(graphic::cNode *node, const graphic::ePickState::Enum state) = 0;
	};


	class cPickManager
	{
	public:
		struct ePickMode {enum Enum { SINGLE, MULTI};};

		cPickManager();
		virtual ~cPickManager();	

		void SetMode(const ePickMode::Enum mode);
		bool Add(cNode *node);
		bool Remove(cNode *node);
		bool Pick(float deltaSeconds, const POINT &mousePt, const ePickState::Enum state);
		bool AddListener(iPickListener *listener);
		bool RemoveListener(iPickListener *listener);
		void Clear();


	protected:
		void BoradcastPickEvent(cNode *node, const ePickState::Enum state);


	public:
		ePickMode::Enum m_mode; // default : SINGLE_PICK
		POINT m_offset;
		vector<cNode*> m_nodes;
		vector<iPickListener*> m_listeners;
	};

}