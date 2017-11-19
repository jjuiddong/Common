//
// Action 정의
// 행동을 처리하는 클래스다. cAction 클래스는 추상클래스로 아무 행동도
// 하지 않는다. 구체적인 행동은 이 클래스를 상속받아서 처리한다.
// Action 트리를 관리하기 위한 인터페이스 와
// 행동의 시작과 끝, 루프, 메세지를 처리하는 인터페이스가 정의된다.
//
// 행동을 처리하기 위한 가장 기본적인 기능과 인터페이스를 정의한다.
//
// 2016-05-14
//		- template class 로 수정
//
#pragma once

#include "../aidef.h"


namespace ai
{

	template <class T>
	class cAction
	{
	public:
		cAction(
			iActorInterface<T> *agent = NULL,
			const StrId &name = "action",
			const StrId &animationName = "",
			const eActionType::Enum type = eActionType::NONE);
		virtual ~cAction();

		virtual bool Traverse(const float deltaSeconds);
		virtual bool ActionThrough(const float deltaSeconds);
		virtual bool ActionExecute(const float deltaSeconds);
		virtual bool StartAction();
		virtual bool MessageProccess(const sMsg &msg);
		virtual void Clear();

		void PushAction(cAction *action);
		void PushFrontAction(cAction *action);
		void PopAction();
		void RemoveAction(cAction *action);
		void RemoveAction(const StrId &actionName);
		void ClearChildAction();
		cAction* FindAction(const StrId &actionName, bool isFindChild=true);
		cAction* FindAction(const eActionType::Enum type, bool isFindChild=true);
		bool IsCurrentAction(const eActionType::Enum type);
		bool IsAction(const eActionType::Enum type);
		cAction* GetLeafAction();
		virtual int GetSubState() const;
		void SetActionState(const eActionState::Enum state);
		bool operator== (const cAction &rhs);


	protected:
		void NextAction();
		cAction* GetNextAction();


	public:
		eActionState::Enum m_state;
		eActionType::Enum m_type;
		StrId m_name;
		StrId m_animationName;
		iActorInterface<T> *m_agent;
		vector<cAction<T>*> m_children;
		cAction<T> *m_current;
	};


	//----------------------------------------------------------------
	// Implements

	template<class T>
	cAction<T>::cAction(
		iActorInterface<T> *agent = NULL,
		const StrId &name = "action",
		const StrId &animationName = "",
		const eActionType::Enum type = eActionType::NONE)
		: m_agent(agent)
		, m_name(name)
		, m_animationName(animationName)
		, m_type(type)
		, m_state(eActionState::WAIT)
		, m_current(NULL)
	{
	}

	template<class T>
	cAction<T>::~cAction()
	{
		Clear();
	}


	// Action이 실행 중이라면 true를 리턴하고, 
	// 아무 작업도 하지 않는다면 false 를 리턴한다.
	template<class T>
	bool cAction<T>::Traverse(const float deltaSeconds)
	{
		if (m_state != eActionState::RUN)
			if (!StartAction())
				NextAction();

		m_state = eActionState::RUN;

		if (m_current)
		{
			if (!ActionThrough(deltaSeconds))
				return true;

			if (!m_current->Traverse(deltaSeconds))
				NextAction();
		}
		else
		{
			if (!ActionExecute(deltaSeconds))
				return false;
		}

		return true;
	}


	// false를 리턴하면 child의 Move()는 호출하지 않는다. 
	// 파생된 클래스에서 구현한다.
	template<class T>
	bool cAction<T>::ActionThrough(const float deltaSeconds)
	{
		return true;
	}

	// Leaf노드 일때만 호출된다.
	// 행동을 처리할 때 여기에 코딩을 하면된다.
	// false 를 리턴하면 행동이 종료되었다는 뜻이다.
	// 파생된 클래스에서 구현한다.
	template<class T>
	bool cAction<T>::ActionExecute(const float deltaSeconds)
	{
		return true;
	}


	// 행동이 처음 시작될 때 호출된다.
	// 파생된 클래스에서 구현한다.
	// false를 리턴하면, 액션을 종료한다.
	template<class T>
	bool cAction<T>::StartAction()
	{
		return true;
	}


	// 실행 중인 자식 Action에 메세지를 보낸다.
	// 오버라이딩해서 구현한다.
	template<class T>
	bool cAction<T>::MessageProccess(const sMsg &msg)
	{
		if (m_current)
			m_current->MessageProccess(msg);
		return true;
	}


	template<class T>
	void cAction<T>::Clear()
	{
		m_state = eActionState::STOP;
		ClearChildAction();
	}


	// Action을 스택에 추가한다.
	template<class T>
	void cAction<T>::PushAction(cAction *action)
	{
		action->m_agent = m_agent;
		m_children.push_back(action);

		// currentAction 재설정한다. currentAction은 가장 최근 값이어야 한다.
		m_current = action;
	}


	// Action을 스택 Top에 추가한다.
	template<class T>
	void cAction<T>::PushFrontAction(cAction *action)
	{
		action->m_agent = m_agent;
		m_children.push_back(action);
		rotateright(m_children);

		if (m_children.size() == 1)
		{
			// Action이 하나밖에 없다면, currentAction으로 설정한다.
			m_current = action;
		}
	}


	// Action을 제거하고, 새 Action 을 current에 할당한다.
	template<class T>
	void cAction<T>::PopAction()
	{
		if (m_current)
		{
			delete m_current;
		}
		m_current = GetNextAction();
	}


	// 인자로 넘어온 action의 메모리까지 제거한다.
	template<class T>
	void cAction<T>::RemoveAction(cAction *action)
	{
		for (u_int i = m_children.size() - 1; i >= 0; --i)
		{
			if (*action == *m_children[i])
			{
				delete m_children[i];
				rotatepopvector(m_children, i);
			}
		}

		// currentAction 재설정한다.
		m_current = m_children.empty() ? NULL : m_children.back();
	}


	template<class T>
	void cAction<T>::RemoveAction(const StrId &actionName)
	{
		for (u_int i = m_children.size() - 1; i >= 0; --i)
		{
			if (actionName == m_children[i]->m_name)
			{
				delete m_children[i];
				rotatepopvector(m_children, i);
			}
		}

		// currentAction 재설정한다.
		m_current = m_children.empty() ? NULL : m_children.back();
	}


	// 자식으로 추가된 Action을 제거한다.
	template<class T>
	void cAction<T>::ClearChildAction()
	{
		for each (auto action in m_children)
		{
			delete action;
		}
		m_children.clear();
		m_current = NULL;
	}


	template<class T>
	cAction<T>* cAction<T>::FindAction(const StrId &actionName, bool isFindChild = true)
	{
		for (u_int i = 0; i < m_children.size(); ++i)
		{
			if (actionName == m_children[i]->m_name)
				return m_children[i];
		}

		if (isFindChild)
		{
			for (u_int i = 0; i < m_children.size(); ++i)
			{
				if (cAction *action = m_children[i]->FindAction(actionName, isFindChild))
					return action;
			}
		}

		return NULL;
	}


	template<class T>
	cAction<T>* cAction<T>::FindAction(const eActionType::Enum type, bool isFindChild = true)
	{
		for (u_int i = 0; i < m_children.size(); ++i)
		{
			if (m_children[i]->IsCurrentAction(type))
				return m_children[i];
		}

		if (isFindChild)
		{
			for (u_int i = 0; i < m_children.size(); ++i)
			{
				if (cAction *action = m_children[i]->FindAction(type, isFindChild))
					return action;
			}
		}

		return NULL;
	}


	template<class T>
	bool cAction<T>::IsCurrentAction(const eActionType::Enum type)
	{
		return m_type == type;
	}


	// Action 타입을 비교한다. this 와 current 중에 
	// 같은 의 타입이 있다면 true 를 리턴한다.
	template<class T>
	bool cAction<T>::IsAction(const eActionType::Enum type)
	{
		if (IsCurrentAction(type))
			return true;
		if (m_current)
			return m_current->IsAction(type);
		return false;
	}


	// Leaf 노드를 리턴한다.
	template<class T>
	cAction<T>* cAction<T>::GetLeafAction()
	{
		cAction *action = m_current;
		if (!action)
			return this;
		return action->GetLeafAction();
	}


	template<class T>
	int cAction<T>::GetSubState() const
	{
		return 0;
	}


	template<class T>
	void cAction<T>::SetActionState(const eActionState::Enum state)
	{
		m_state = state;
	}


	template<class T>
	bool cAction<T>::operator== (const cAction &rhs)
	{
		return m_name == rhs.m_name;
	}


	// 다음 액션으로 넘어간다.
	template<class T>
	void cAction<T>::NextAction()
	{
		cAction *prevAction = m_current;
		m_current = GetNextAction();
		SAFE_DELETE(prevAction);
	}


	// 다음 액션을 리턴한다.
	template<class T>
	cAction<T>* cAction<T>::GetNextAction()
	{
		if (m_children.empty())
			return NULL;
		m_children.pop_back();
		if (m_children.empty())
			return NULL;
		return m_children.back();
	}

}
