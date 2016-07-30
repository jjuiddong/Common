//
// 2016-07-17, jjuiddong
// Fraps 에서 캡쳐한 이미지를 로딩한다.
// 
// 쓰레드를 돌면서, Fraps 디렉토리를 검사해 파일이 생성되면 로딩한다.
// 로딩된 이미지는 제거한다.
//
#pragma once


namespace cvproc
{
	class cFrapsCapture
	{
	public:
		cFrapsCapture();
		virtual ~cFrapsCapture();

		bool Init(const string &frapsScreenShotPath, const int key);
		void ScreenShot();
		bool IsUpdateScreenShot();
		void Clear();


	public:
		// 쓰레드 전용 함수들
		void KeyDown();
		bool SearchFrapsDir();


	public:
		enum STATE {
			WAIT,  // 대기중
			SCREENSHOT,	// 스크린샷 요청
			KEYDOWN, // 스크린샷 키 다운
			SEARCH_DIR, // Fraps 디렉토리 검색
			END, // 스크린샷 이미지 가져오기 성공.
		};
		
		STATE m_state;
		string m_frapsScreenShotPath;
		cv::Mat m_img; // 스크린샷 이미지
		int m_key; // Fraps 스크린샷 키
		int m_keyDownDelay; // default : 100 milliseconds
		list<string> m_exts; // *.bmp

		// 쓰레드 변수
		std::thread m_thread;
		bool m_isRunThread;
	};
}
