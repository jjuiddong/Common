//
// 2016-05-01, jjuiddong
// Action을 처리하는 인공지능 객체
// Actor 는 트리행태다.  자식으로 Actor를 가질 수 있다.
//
#pragma once

// #include "../object/actorinterface.h"
// #include "../action/action.h"
//#include "../control/event.h"


namespace ai
{

	template <class T>

	class cActor : public cObject
	{
	public:
		cActor(iActorInterface<T> *agent =NULL)
			: m_agent(agent)
			//, m_id(common::GenerateId())
		{
		}

		virtual ~cActor()
		{
			Clear();
		}

		virtual bool Init()
		{
			return true;
		}

		virtual bool Update(const float deltaSeconds)
		{
			if (m_rootAction)
				m_rootAction->Traverse(deltaSeconds);
			return true;
		}

		virtual void Clear()
		{
			ClearAction();
			ClearChildActor();
		}


		// Action
		void SetAction(cAction<T> *action)
		{
			RET(!action);

			if (m_rootAction)
				ClearAction();

			action->m_agent = m_agent;
			m_rootAction = new cRootAction<T>(m_agent);
			m_rootAction->PushAction(action);
		}


		void PushAction(cAction<T> *action)
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


		void PushFrontAction(cAction<T> *action)
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

		
		cAction<T>* GetAction()
		{
			RETV(!m_rootAction, NULL);
			return m_rootAction->m_current;
		}

		bool IsAction(ACTION_TYPE::TYPE type)
		{
			RETV(!m_rootAction, false);
			return m_rootAction->IsAction(type);
		}

		void ClearAction()
		{
			SAFE_DELETE(m_rootAction);
		}

		// Children Actor
		bool AddActor(cObject *actor)
		{
			if (IsExistActor(actor))
				return false;

			m_children.insert(ChildType::value_type(actor->m_id, actor));
			return true;
		}


		bool RemoveActor(const int actorId)
		{
			return m_children.remove(actorId);
		}


		cObject* GetActor(const int actorId)
		{
			auto it = m_children.find(actorId);
			if (m_children.end() == it)
				return NULL;
			return it->second;
		}
		
		
		bool IsExistActor(cObject *actor)
		{
			return IsExistActor(actor->m_id) ? true : false;
		}


		bool IsExistActor(const int actorId)
		{
			return GetActor(actorId) ? true : false;
		}


		// 메모리는 외부에서 제거한다.
		void ClearChildActor()
		{
			m_children.clear();
		}

		
		virtual void DispatchMessage(const sMsg &msg) override
		{
			if (m_rootAction)
				m_rootAction->MessageProccess(msg);

			// children message loop
			for each (auto &actor in m_children.m_Seq)
			{
				actor->DispatchMessage(msg);
			}
		}


		bool operator==(const cActor &rhs)
		{
			return Equal(rhs);
		}

		bool operator<(const cActor &rhs)
		{
			return m_id < rhs.m_id;
		}

		bool Equal(const cActor &rhs)
		{
			return rhs.m_id == m_id;
		}


	public:
		iActorInterface<T> *m_agent;
		cAction<T> *m_rootAction;

		typedef VectorMap<int, cObject* > ChildType;
		ChildType m_children;
	};

}
