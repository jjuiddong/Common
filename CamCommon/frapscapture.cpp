
#include "stdafx.h"
#include "frapscapture.h"

using namespace cv;
using namespace cvproc;

void FrapsThreadProc(cFrapsCapture*);


cFrapsCapture::cFrapsCapture()
	: m_state(WAIT)
	, m_keyDownDelay(100)
{
	m_exts.push_back("bmp");
	m_exts.push_back("jpg");
}

cFrapsCapture::~cFrapsCapture()
{
	Clear();
}


bool cFrapsCapture::Init(const string &frapsScreenShotPath, const int key)
{
	Clear();

	m_frapsScreenShotPath = frapsScreenShotPath;
	m_key = key;

	m_isRunThread = true;
	m_thread = std::thread(FrapsThreadProc, this);

	return true;
}


void cFrapsCapture::ScreenShot()
{
	m_state = SCREENSHOT;
}


// 스크린샷이 업데이트 되었다면, true를 리턴
bool cFrapsCapture::IsUpdateScreenShot()
{
	return (m_state == END);
}


void cFrapsCapture::Clear()
{
	if (m_thread.joinable())
	{
		m_isRunThread = false;
		m_thread.join();
	}
}


void cFrapsCapture::KeyDown()
{
	INPUT input;
	//WORD vkey = VK_SNAPSHOT; // see link below
	WORD vkey = m_key;
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(m_keyDownDelay);

	input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(m_keyDownDelay);
}


// Fraps 스크린샷 경로에 파일이 업데이트 되었는지 확인한다.
// 확인된 파일은 로드하고 지운다. 나머지 스크린샷 파일들도 지운다.
// *.bmp 파일을 찾는다.
//
// return value : 파일을 찾았다면 true를 리턴.
//
bool cFrapsCapture::SearchFrapsDir()
{
	list<string> out;
	CollectFiles(m_exts, m_frapsScreenShotPath, out);

	if (out.empty())
		return false;

	m_img = imread(out.back());

	for each (auto file in out)
		DeleteFileA(file.c_str());

	return true;
}


// Fraps 디렉토리를 검사해서, 스크린샷 이미지가 있으면 가져온다.
void FrapsThreadProc(cFrapsCapture *capture)
{
	while (capture->m_isRunThread)
	{

		switch (capture->m_state)
		{
		case cFrapsCapture::WAIT:  // 대기중
			Sleep(30);
			break;

		case cFrapsCapture::SCREENSHOT:	// 스크린샷 요청
			capture->m_state = cFrapsCapture::KEYDOWN;
			break;

		case cFrapsCapture::KEYDOWN: // 스크린샷 키 다운
			capture->KeyDown();
			capture->m_state = cFrapsCapture::SEARCH_DIR;
			break;

		case cFrapsCapture::SEARCH_DIR: // Fraps 디렉토리 검색
		{
			int cnt = 0; // 2초 동안 검사한다.
			while (!capture->SearchFrapsDir() && (cnt < 20))
			{
				++cnt;
				Sleep(30);
			}
			capture->m_state = cFrapsCapture::END;
		}
		break;

		case cFrapsCapture::END: // 스크린샷 이미지 가져오기 종료.
			Sleep(30);
			break;

		default:
			assert(0);
			break;
		}

	}
}
