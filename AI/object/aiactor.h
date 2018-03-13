//
// 2016-05-01, jjuiddong
// Action을 처리하는 인공지능 객체
// Actor 는 트리행태다.  자식으로 Actor를 가질 수 있다.
//
#pragma once


namespace ai
{
	using namespace common;

	template <class T>
	class cActor : public cObject
	{
	public:
		cActor(iActorInterface<T> *agent = NULL);
		virtual ~cActor();

		virtual bool Init();
		virtual bool Update(const float deltaSeconds);
		virtual void Clear();

		// Action
		void SetAction(cAction<T> *action);
		void PushAction(cAction<T> *action);
		void PushFrontAction(cAction<T> *action);
		cAction<T>* GetAction();
		bool IsAction(const eActionType::Enum type);
		bool IsCurrentAction(const eActionType::Enum type);
		void ClearAction();

		// Children Actor
		bool AddActor(cObject *actor);
		bool RemoveActor(const int actorId);
		cObject* GetActor(const int actorId);
		bool IsExistActor(cObject *actor);
		bool IsExistActor(const int actorId);
		void ClearChildActor();
		virtual void DispatchMsg(const sMsg &msg) override;

		bool operator==(const cActor &rhs);
		bool operator<(const cActor &rhs);
		bool Equal(const cActor &rhs);


	public:
		iActorInterface<T> *m_agent;
		cAction<T> *m_rootAction;
		typedef VectorMap<int, cObject* > ChildType;
		ChildType m_children;
	};


	//---------------------------------------------------------------
	// Implements
	
	template <class T>
	cActor<T>::cActor(iActorInterface<T> *agent //= NULL
	)
		: m_agent(agent)
	{
	}


	template <class T>
	cActor<T>::~cActor()
	{
		Clear();
	}


	template <class T>
	bool cActor<T>::Init()
	{
		return true;
	}


	template <class T>
	bool cActor<T>::Update(const float deltaSeconds)
	{
		if (m_rootAction)
			m_rootAction->Traverse(deltaSeconds);
		return true;
	}


	template <class T>
	void cActor<T>::Clear()
	{
		ClearAction();
		ClearChildActor();
	}


	// Action
	template <class T>
	void cActor<T>::SetAction(cAction<T> *action)
	{
		RET(!action);

		if (m_rootAction)
			ClearAction();

		action->m_agent = m_agent;
		m_rootAction = new cRootAction<T>(m_agent);
		m_rootAction->PushAction(action);
	}


	template <class T>
	void cActor<T>::PushAction(cAction<T> *action)
	{
		RET(!action);

		if (m_rootAction)
		{
			action->m_agent = m_agent;
			m_rootAction->PushAction(action);
		}
		else
		{
			SetAction(action);
		}
	}


	template <class T>
	void cActor<T>::PushFrontAction(cAction<T> *action)
	{
		RET(!action);

		if (m_rootAction)
		{
			action->m_agent = m_agent;
			m_rootAction->PushFrontAction(action);
		}
		else
		{
			SetAction(action);
		}
	}


	template <class T>
	cAction<T>* cActor<T>::GetAction()
	{
		RETV(!m_rootAction, NULL);
		return m_rootAction->GetLeafAction();
	}


	template <class T>
	bool cActor<T>::IsAction(const eActionType::Enum type)
	{
		RETV(!m_rootAction, false);
		return m_rootAction->IsAction(type);
	}


	template <class T>
	bool cActor<T>::IsCurrentAction(const eActionType::Enum type)
	{
		RETV(!m_rootAction, false);
		return m_rootAction->GetLeafAction()->IsCurrentAction(type);
	}


	template <class T>
	void cActor<T>::ClearAction()
	{
		SAFE_DELETE(m_rootAction);
	}


	// Children Actor
	template <class T>
	bool cActor<T>::AddActor(cObject *actor)
	{
		if (IsExistActor(actor))
			return false;

		m_children.insert(ChildType::value_type(actor->m_id, actor));
		return true;
	}


	template <class T>
	bool cActor<T>::RemoveActor(const int actorId)
	{
		return m_children.remove(actorId);
	}


	template <class T>
	cObject* cActor<T>::GetActor(const int actorId)
	{
		auto it = m_children.find(actorId);
		if (m_children.end() == it)
			return NULL;
		return it->second;
	}


	template <class T>
	bool cActor<T>::IsExistActor(cObject *actor)
	{
		return IsExistActor(actor->m_id) ? true : false;
	}


	template <class T>
	bool cActor<T>::IsExistActor(const int actorId)
	{
		return GetActor(actorId) ? true : false;
	}


	// 메모리는 외부에서 제거한다.
	template <class T>
	void cActor<T>::ClearChildActor()
	{
		m_children.clear();
	}


	template <class T>
	void cActor<T>::DispatchMsg(const sMsg &msg)
	{
		if (m_rootAction)
			m_rootAction->MessageProccess(msg);

		// children message loop
		for each (auto &actor in m_children.m_Seq)
		{
			actor->DispatchMsg(msg);
		}
	}


	template <class T>
	bool cActor<T>::operator==(const cActor &rhs)
	{
		return Equal(rhs);
	}


	template <class T>
	bool cActor<T>::operator<(const cActor &rhs)
	{
		return m_id < rhs.m_id;
	}


	template <class T>
	bool cActor<T>::Equal(const cActor &rhs)
	{
		return rhs.m_id == m_id;
	}

}
