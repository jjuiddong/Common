//
// 2016-06-02, jjuiddong
// 씬을 이동하는 기능을 한다.
// 씬 이동시, ESC, Enter 키를 눌러 이동 한다.
// cSceneTraverse 는 씬 이동에 따른 키를 반환한다.
//
#pragma once


namespace cvproc {
	namespace imagematch {

		class cSceneTraverse
		{
		public:
			cSceneTraverse();
			virtual ~cSceneTraverse();

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


			bool Init(const string &imageMatchScriptFileName, 
				const string &flowScriptFileName, 
				const int screenCaptureKey=VK_SNAPSHOT);
			STATE Update(const float deltaSeconds, const cv::Mat &img, OUT int &key);
			void Move(const string &sceneName);
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
			STATE OnProc(const cv::Mat &img, OUT int &key);
			STATE OnMenu(const cv::Mat &img, OUT int &key);
			STATE OnMenuDetect(const cv::Mat &img, OUT int &key);

			bool TreeMatch(cGraphScript::sNode *current, const cv::Mat &img, OUT sTreeMatchData &out, 
				const int loopCnt=-1);
			bool CheckMenuUpandDown(const cGraphScript::sNode *current, const cGraphScript::sNode *next);
			int GetNextMenuCount(const cGraphScript::sNode *current, const cGraphScript::sNode *next);
			int CheckNextMenuIndex(const cGraphScript::sNode *current, const cGraphScript::sNode *next);
			int CheckNextMenuIndex(const cGraphScript::sNode *current, const string &selectMenuId, 
				const cGraphScript::sNode *next);
			void Delay(const float delaySeconds, const STATE nextState);
			string GenerateInputID();
			cMatchResult& GetMatchResult();


		public:
			STATE m_state; // 현재 상태
			bool m_isLog; // 로그 출력
			cMatchScript2 m_matchScript;
			cGraphScript m_flowScript;
			cGraphScript::sNode *m_currentNode;
			cGraphScript::sNode *m_nextNode;
			vector<cGraphScript::sNode*> m_path; // 이동 경로
			string m_moveTo;	// 이동할 씬 이름
			int m_tryMachingCount;
			int m_screenCaptureKey; // virtual key, VK_SNAPSHOT or VK_F11
			int m_genId; // default = 100000

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

} }