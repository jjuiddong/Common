//
// 2017-03-14, jjuiddong
// Picking Check, Notify Event
//
// Refactoring, 2017-10-15
//
#pragma once


namespace graphic
{

	class cPickManager
	{
	public:
		struct ePickMode {enum Enum { SINGLE, MULTI};};

		cPickManager();
		virtual ~cPickManager();	

		void SetMode(const ePickMode::Enum mode);
		bool Add(cNode2 *node);
		bool Remove(cNode2 *node);
		bool Pick(float deltaSeconds, const POINT &mousePt);
		void Clear();


	public:
		ePickMode::Enum m_mode; // default : SINGLE_PICK
		cCamera *m_mainCamera; // reference
		POINT m_offset;
		vector<cNode2*> m_nodes;
	};

}