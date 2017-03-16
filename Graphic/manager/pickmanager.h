//
// 2017-03-14, jjuiddong
// Picking Check, Notify Event
//
#pragma once


namespace graphic
{
	interface iPickable;

	class cPickManager : public common::cSingleton<cPickManager>
	{
	public:
		struct ePickMode {enum Enum { SINGLE, MULTI};};

		cPickManager();
		virtual ~cPickManager();	

		void SetMode(const ePickMode::Enum mode);
		bool Add(iPickable *obj);
		bool Remove(iPickable *obj);
		void Update(const float deltaSeconds);
		void Clear();


	public:
		ePickMode::Enum m_mode; // default : SINGLE_PICK
		vector<iPickable*> m_objects;
	};

}