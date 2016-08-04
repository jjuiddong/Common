//
// 2016-08-03, jjuiddong
// 메뉴를 이동하면서, 세팅하는 기능을 함.
//
// menu setting command
// category1 sub_category1 item1
// category2 sub_category2 item2
//
#pragma once


namespace cvproc {
	namespace imagematch {

		class cMenuControl
		{
		public:
			cMenuControl();
			virtual ~cMenuControl();

			enum STATE {
				WAIT, // 대기 중
				DELAY, // 일정시간 동안 지연
				REACH, // 목적지 도착
				CAPTURE, // 영상 캡쳐 요청, 캡쳐 후, 항상 PROC 상태가 된다.
				PROC, // 영상을 입력받아, 목적지를 이동하기 위한 key 를 계산
				MENU_MOVE, // 메뉴 이동 중. 상하좌우
				MENU_DETECT,	// 메뉴 확인 및 이동
				ERR, // 연산 중, 에러 발생 
			};

			bool Init(
				cMatchScript2 *matchScript, 
				const string &menuControlScriptFileName, 
				const int screenCaptureKey = VK_SNAPSHOT);

			STATE Update(const float deltaSeconds, const cv::Mat &img, OUT int &key);
			bool StartMenuSetting(const string &menuHeadName, const string &cmd);
			void Cancel();


		protected:
			STATE OnDelay(const float deltaSeconds, const cv::Mat &img, OUT int &key);
			STATE OnCapture(const cv::Mat &img, OUT int &key);
			STATE OnProc(const cv::Mat &img, OUT int &key);
			STATE OnMenu(const cv::Mat &img, OUT int &key);
			STATE OnMenuDetect(const cv::Mat &img, OUT int &key);


		public:
			STATE m_state; // 현재 상태
			bool m_isLog; // 로그 출력
			cMatchScript2 *m_matchScript;
			cGraphScript m_menuScript;
			cGraphScript::sNode *m_currentNode;
			string m_headName;
			int m_screenCaptureKey; // virtual key, VK_SNAPSHOT or VK_F11

			// menu setting command
			int m_currentCmdIdx;
			vector<string> m_cmds;

			// menu move
			int m_nextMenuIdx;
			int m_currentMenuIdx;

			// delay
			STATE m_nextState; // 딜레이가 끝난 후, 상태 
			bool m_isInitDelay; // 한프레임이 지난 후부터 딜레이 계산을 시작하기 위해 쓰임
			float m_delayTime;
			float m_initDelayTime;

			// matchresult buffer
			int m_matchResultBuffIdx;
			cMatchResult m_matchResult[16];
		};

	}
}