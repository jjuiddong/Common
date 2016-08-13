//
// 2016-06-02, jjuiddong
// 씬을 이동하는 기능을 한다.
// 씬 이동시, ESC, Enter 키를 눌러 이동 한다.
// cFlowControl 는 씬 이동에 따른 키를 반환한다.
//
#pragma once


namespace cvproc {
	namespace imagematch {

		class cFlowControl
		{
		public:
			cFlowControl();
			virtual ~cFlowControl();

			enum STATE {
				WAIT, // 대기 중
				DELAY, // 일정시간 동안 지연
				REACH, // 목적지 도착
				CAPTURE, // 영상 캡쳐 요청, 캡쳐 후, 항상 PROC 상태가 된다.
				CAPTURE_ERR, // 영상 캡쳐 요청, 인식에 실패해서, 재 계산한다.
				CAPTURE_NEXT, // 영상 캡쳐 요청, 캡처 후, 다음 씬으로 넘어간다.
				CAPTURE_MENU, // 영상 캡쳐 요청, 캡처 후, 현재 메뉴 상태를 확인하고, 다음 단계로 넘어간다.
				PROC, // 영상을 입력받아, 현재 위치를 파악한 후, 목적지를 이동하기 위한 key 를 계산
				NEXT, // 영상을 입력받아,  다음 설정 메뉴로 넘어간다. 현재 위치는 파악하지 않는다.
				MENU_MOVE, // 메뉴 이동 중. 상하좌우
				MENU_DETECT,	// 메뉴 확인 및 이동
				ERR, // 연산 중, 에러 발생 
			};


			bool Init(const string &imageMatchScriptFileName, 
				const string &flowScriptFileName, 
				const int screenCaptureKey=VK_SNAPSHOT);
			STATE Update(const float deltaSeconds, const cv::Mat &img, OUT int &key);
			bool Command(const string &cmdFileName);
			void Cancel();
			

		protected:
			struct sTreeMatchData
			{
				cGraphScript::sNode *node;
				string result;
				int loopCnt;
			};

			STATE OnDelay(const float deltaSeconds, const cv::Mat &img, OUT int &key);
			STATE OnCapture(const cv::Mat &img, OUT int &key);
			STATE OnCaptureNext(const cv::Mat &img, OUT int &key);
			STATE OnCaptureMenu(const cv::Mat &img, OUT int &key);
			STATE OnProc(const cv::Mat &img, OUT int &key);
			STATE OnMenu(const cv::Mat &img, OUT int &key);
			STATE OnMenuDetect(const cv::Mat &img, OUT int &key);
			STATE OnNext(const cv::Mat &img, OUT int &key);
			STATE NextStep(const cv::Mat &img, cGraphScript::sNode *detectNode, OUT int &key);

			bool TreeMatch(cGraphScript::sNode *current, const cv::Mat &img, OUT sTreeMatchData &out, 
				const int loopCnt=-1);
			int CheckMenuUpandDown(const cGraphScript::sNode *current, const cGraphScript::sNode *next);
			int GetNextMenuCount(const cGraphScript::sNode *current, const cGraphScript::sNode *next);
			int CheckNextMenuIndex(const cGraphScript::sNode *current, const cGraphScript::sNode *next);
			int CheckNextMenuIndex(const cGraphScript::sNode *current, const string &selectMenuId, 
				const cGraphScript::sNode *next);
			STATE Delay(const float delaySeconds, const STATE nextState);
			string GenerateInputID();
			cMatchResult& GetMatchResult();
			string GetStateString(const STATE state);
			STATE NextCommand();


		public:
			STATE m_state; // 현재 상태
			bool m_isLog; // 로그 출력
			cMatchScript2 m_matchScript;
			cGraphScript m_flowScript;
			cGraphScript::sNode *m_detectNode;
			cGraphScript::sNode *m_nextNode;
			vector<cGraphScript::sNode*> m_path; // 이동 경로
			vector<string> m_commands;
			int m_cmdIdx; // 실행중인 m_commands Index
			int m_tryMachingCount;
			int m_screenCaptureKey; // virtual key, VK_SNAPSHOT or VK_F11
			int m_genId; // default = 100000
			cv::Mat m_lastImage;

			// menu move
			int m_nextMenuIdx;
			int m_currentMenuIdx;
			bool m_isMenuCheck; // 다음으로 넘어가기 전에 메뉴를 한번 확인한다.

			// delay
			STATE m_nextState; // 딜레이가 끝난 후, 상태 
			bool m_isInitDelay; // 한프레임이 지난 후부터 딜레이 계산을 시작하기 위해 쓰임
			float m_delayTime;
			float m_initDelayTime;

			// matchresult buffer
			int m_matchResultBuffIdx;
			cMatchResult m_matchResult[16];
		};

} }