
#include "stdafx.h"
#include "movie.h"
#include <process.h>


using namespace cv;

unsigned WINAPI MovieThreadFunction(void* arg);


cMovie::cMovie() :
	m_width(0)
	, m_height(0)
	, m_isThreadMode(false)
	, m_threadLoop(true)
{
	InitializeCriticalSectionAndSpinCount(&m_criticalSection, 0x00000400);

}

cMovie::~cMovie()
{
	m_threadLoop = false;
	::WaitForSingleObject(m_handle, 1000);
	DeleteCriticalSection(&m_criticalSection);

	cvDestroyWindow(m_name.c_str());
}


// 클래스 초기화
bool cMovie::Init(const string &name, const int width, const int height,
	const bool isThreadMode, const bool isFullScreen)
	// isThread = false, isFullScreen = false
{
	m_name = name;
	m_width = width;
	m_height = height;
	m_isThreadMode = isThreadMode;

	cvNamedWindow(name.c_str(), 0);
	cvResizeWindow(name.c_str(), width, height);
	if (isFullScreen)
		cvSetWindowProperty(name.c_str(), CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

	if (isThreadMode)
	{
		m_handle = (HANDLE)_beginthreadex(NULL, 0, MovieThreadFunction, this, 0, (unsigned*)&m_threadId);
	}

	return true;
}


// 출력할 영상을 복사한다.
void cMovie::Draw(const cv::Mat &image)
{
	AutoCS cs(&m_criticalSection);
	m_image = image.clone();
}


// DxShow 쓰레드
unsigned WINAPI MovieThreadFunction(void* arg)
{
	cMovie *movie = (cMovie*)arg;

	while (movie->m_threadLoop)
	{
		AutoCS cs(&movie->m_criticalSection);
		imshow(movie->m_name.c_str(), movie->m_image);

		cvWaitKey(1);
		Sleep(1);
	}

	return 0;
}
