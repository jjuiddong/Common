// 인공지능에 필요한 타입들을 선언한다.
// 가장 먼저 선언되는 파일이기 때문에 인공지능에서 쓰이는 모든 클래스는
// 여기에 정의된 타입들을 가져다 쓸 수 있다.
#pragma once


namespace ai
{
	using namespace common;

	class cObject;

	struct eActionState {
		enum Enum {
			WAIT
			, RUN
			, STOP
		};
	};

	struct eActionType {
		enum Enum {
			NONE
			, ROOT
			
			, WAIT
			, MOVE
			, GROUP_MOVE
			, ATTACK
			, PATROL
			
			,MAX_ACTION_TYPE0,
		};
	};

	struct eMsgType {
		enum Enum {
			NONE

			, UNIT_SIDEMOVE
				// unit - unit collision message
		};
	};


	struct sMsg
	{
		cObject *receiver;
		cObject *sender;
		eMsgType::Enum msg;
		int param1;
		int param2;
		Vector3 v;
		int sendTime;
		//char comment[32];

		sMsg(
			cObject *rcv = NULL
			, cObject *snd = NULL
			, eMsgType::Enum msg0 = eMsgType::NONE
			, int parameter1 = 0
			, int parameter2 = 0
			, const Vector3 &v0 = Vector3(0,0,0)
			, int time = 0
		) 
			: receiver(rcv)
			, sender(snd)
			, msg(msg0)
			, param1(parameter1)
			, param2(parameter2)
			, v(v0)
			, sendTime(time)
		{
			//comment[0]=NULL;
		}
	};

}
