//
// 2016-05-01, jjuiddong
// Action을 처리하는 인공지능 객체
// Brain은 트리행태다. 자식으로 Brain을 가질 수 있다.
//
// 2018-10-20
//	- naming change, cActor -> cBrain
//
#pragma once


namespace ai
{
	using namespace common;

	template <class T>
	class cBrain : public cObject
	{
	public:
		cBrain(T *agent = NULL);
		virtual ~cBrain();

		virtual bool Init();
		virtual bool Update(const float deltaSeconds) override;
		virtual void DispatchMsg(const sMsg &msg) override;
		virtual void Clear();

		// Action
		void SetAction(cAction<T> *action);
		void PushAction(cAction<T> *action);
		void PushFrontAction(cAction<T> *action);
		cAction<T>* GetAction();
		bool IsAction(const eActionType::Enum type);
		bool IsCurrentAction(const eActionType::Enum type);
		void ClearAction();

		// Children Brain
		bool AddBrain(cObject *brain);
		bool RemoveBrain(const int brainId);
		cObject* GetBrain(const int brainId);
		bool IsExistBrain(cObject *brain);
		bool IsExistBrain(const int brainId);
		void ClearChildBrain();

		// Message
		void PostMsg(const sMsg &msg);
		void SendMsg(const sMsg &msg);

		bool operator==(const cBrain &rhs);
		bool operator<(const cBrain &rhs);
		bool Equal(const cBrain &rhs);


	public:
		T *m_agent;
		cAction<T> *m_rootAction;
		typedef VectorMap<int, cObject* > ChildType;
		ChildType m_children;
	};


	//---------------------------------------------------------------
	// Implements
	
	template <class T>
	cBrain<T>::cBrain(T *agent //= NULL
	)
		: m_agent(agent)
	{
	}


	template <class T>
	cBrain<T>::~cBrain()
	{
		Clear();
	}


	template <class T>
	bool cBrain<T>::Init()
	{
		return true;
	}


	template <class T>
	bool cBrain<T>::Update(const float deltaSeconds)
	{
		if (m_rootAction)
			m_rootAction->Traverse(deltaSeconds);
		
		for (auto &brain : m_children.m_seq)
			brain->Update(deltaSeconds);

		return true;
	}


	template <class T>
	void cBrain<T>::Clear()
	{
		ClearAction();
		ClearChildBrain();
	}


	// Action
	template <class T>
	void cBrain<T>::SetAction(cAction<T> *action)
	{
		RET(!action);

		if (m_rootAction)
			m_rootAction->ClearChildAction();
		if (!m_rootAction)
			m_rootAction = new cRootAction<T>(m_agent);

		action->m_agent = m_agent;
		m_rootAction->PushAction(action);
	}


	template <class T>
	void cBrain<T>::PushAction(cAction<T> *action)
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
	void cBrain<T>::PushFrontAction(cAction<T> *action)
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
	cAction<T>* cBrain<T>::GetAction()
	{
		RETV(!m_rootAction, NULL);
		return m_rootAction->GetLeafAction();
	}


	template <class T>
	bool cBrain<T>::IsAction(const eActionType::Enum type)
	{
		RETV(!m_rootAction, false);
		return m_rootAction->IsAction(type);
	}


	template <class T>
	bool cBrain<T>::IsCurrentAction(const eActionType::Enum type)
	{
		RETV(!m_rootAction, false);
		return m_rootAction->GetLeafAction()->IsCurrentAction(type);
	}


	template <class T>
	void cBrain<T>::ClearAction()
	{
		SAFE_DELETE(m_rootAction);
	}


	// Children Brain
	template <class T>
	bool cBrain<T>::AddBrain(cObject *brain)
	{
		if (IsExistBrain(brain))
			return false;

		m_children.insert(ChildType::value_type(brain->m_id, brain));
		return true;
	}


	template <class T>
	bool cBrain<T>::RemoveBrain(const int brainId)
	{
		return m_children.remove(brainId);
	}


	template <class T>
	cObject* cBrain<T>::GetBrain(const int brainId)
	{
		auto it = m_children.find(brainId);
		if (m_children.end() == it)
			return NULL;
		return it->second;
	}


	template <class T>
	bool cBrain<T>::IsExistBrain(cObject *brain)
	{
		return IsExistBrain(brain->m_id) ? true : false;
	}


	template <class T>
	bool cBrain<T>::IsExistBrain(const int brainId)
	{
		return GetBrain(brainId) ? true : false;
	}


	// 메모리는 외부에서 제거한다.
	template <class T>
	void cBrain<T>::ClearChildBrain()
	{
		m_children.clear();
	}


	template <class T>
	void cBrain<T>::DispatchMsg(const sMsg &msg)
	{
		if (m_rootAction)
			m_rootAction->MessageProccess(msg);

		// children message loop
		for (auto &brain : m_children.m_seq)
			brain->DispatchMsg(msg);
	}


	template <class T>
	void cBrain<T>::PostMsg(const sMsg &msg)
	{
		cMessageManager::Get()->PostMsg(msg);
	}

	template <class T>
	void cBrain<T>::SendMsg(const sMsg &msg)
	{
		cMessageManager::Get()->SendMsg(msg);
	}


	template <class T>
	bool cBrain<T>::operator==(const cBrain &rhs)
	{
		return Equal(rhs);
	}


	template <class T>
	bool cBrain<T>::operator<(const cBrain &rhs)
	{
		return m_id < rhs.m_id;
	}


	template <class T>
	bool cBrain<T>::Equal(const cBrain &rhs)
	{
		return rhs.m_id == m_id;
	}

}
