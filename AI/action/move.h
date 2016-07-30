//
// 2016-05-14, jjuiddong
// �̵� �ൿ.
//
#pragma once


#include "action.h"
#include "../object/actorinterface.h"


namespace ai
{
	template<class T>

	class cMove : public cAction<T>
						, public common::cMemoryPool< cMove<T> >
	{
	public:
		cMove(ai::iActorInterface<T> *agent, const Vector3 &dest, const float speed=3.f)
			: cAction<T>(agent, "move", "zealot_walk.ani", ACTION_TYPE::MOVE)
			, m_rotateTime(0)
			, m_speed(speed)
			, m_rotateInterval(0.3f)
			, m_distance(0)
		{
			m_dest = dest;
			m_rotateTime = 0;

			const Vector3 curPos = agent->aiGetTransform().GetPosition();
			m_distance = curPos.LengthRoughly(m_dest);
			m_oldDistance = curPos.LengthRoughly(m_dest);
			m_dir = m_dest - curPos;
			m_dir.y = 0;
			m_dir.Normalize();

			Quaternion q;
			q.SetRotationArc(Vector3(0, 0, -1), m_dir);

			m_fromDir = agent->aiGetTransform().GetQuaternion();
			m_toDir = q;
			m_rotateTime = 0;

			m_S.SetScale(agent->aiGetTransform().GetScale());
			m_R = m_fromDir.GetMatrix();
		}


		virtual bool ActionExecute(const float deltaSeconds) override
		{
			const Vector3 curPos = m_agent->aiGetTransform().GetPosition();

			// ȸ�� ���� ���
			if (m_rotateTime < m_rotateInterval)
			{
				m_rotateTime += deltaSeconds;

				const float alpha = min(1, m_rotateTime / m_rotateInterval);
				const Quaternion q = m_fromDir.Interpolate(m_toDir, alpha);
				m_R = q.GetMatrix();
			}

			// ĳ���� �̵�.
			const Vector3 pos = curPos + m_dir * m_speed * deltaSeconds;
			m_agent->aiGetTransform() = m_S * m_R;
			m_agent->aiGetTransform().SetPosition(pos);

			// �������� �����ٸ� ����.
			// �������� ���� ��, ����Ǵ� ������ ���� �� �ִ�.
			const float distance = pos.LengthRoughly(m_dest);
			if (pos.LengthRoughly(m_dest) < 0.01f)
				return false; // ������ ����. �׼�����.

			// ������ ���� �ָ� �Դٸ�, �����.
			if (m_oldDistance < distance)
			{
				return false;
			}

			m_oldDistance = distance;
			return true;
		}


	public:
		Vector3 m_dest;
		float m_speed; // 3.f
		float m_rotateInterval; // ȸ�� ���� �ð�, 0.3��
		float m_distance;
		float m_oldDistance;

		Vector3 m_dir; // �̵� ����
		Quaternion m_fromDir; // �̵� �� ����
		Quaternion m_toDir; // �̵� �� ����
		float m_rotateTime; // ȸ�� ���� �ð�.
		Matrix44 m_R;
		Matrix44 m_S;
	};

}