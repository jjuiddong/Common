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
		cPickManager();
		virtual ~cPickManager();

		bool Add(iPickable *obj);
		bool Remove(iPickable *obj);
		void Update(const float deltaSeconds);
		void Clear();


	public:
		vector<iPickable*> m_objects;
	};

}