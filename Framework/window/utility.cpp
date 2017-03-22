
#include "stdafx.h"
#include "utility.h"

using namespace framework;


HWND framework::InitWindow(HINSTANCE hInstance, const wstring &windowName, const RECT &windowRect, int nCmdShow, WNDPROC WndProc,
	const bool dualMonitor)
	// dualMonitor=false
{
	const int X = windowRect.left;
	const int Y = windowRect.top;
	const int WIDTH = windowRect.right - windowRect.left;
	const int HEIGHT = windowRect.bottom - windowRect.top;

	//윈도우 클레스 정보 생성
	//내가 이러한 윈도를 만들겠다 라는 정보
	WNDCLASS WndClass;
	WndClass.cbClsExtra = 0;			//윈도우에서 사용하는 여분의 메모리설정( 그냥 0 이다  신경쓰지말자 )
	WndClass.cbWndExtra = 0;			//윈도우에서 사용하는 여분의 메모리설정( 그냥 0 이다  신경쓰지말자 )
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);		//윈도우 배경색상
	WndClass.hCursor = LoadCursor( NULL, IDC_ARROW );			//윈도우의 커서모양 결정
	WndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );		//윈도우아이콘모양 결정
	WndClass.hInstance = hInstance;				//프로그램인스턴스핸들 
	WndClass.lpfnWndProc = (WNDPROC)WndProc;			//윈도우 프로시져 함수 포인터
	WndClass.lpszMenuName = NULL;						//메뉴이름 없으면 NULL
	WndClass.lpszClassName = windowName.c_str();				//지금 작성하고 있는 윈도우 클레스의 이름
	//WndClass.style	 = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;	//윈도우 그리기 방식 설정 ( 사이즈가 변경될때 화면갱신 CS_HREDRAW | CS_VREDRAW )
	WndClass.style = CS_HREDRAW | CS_VREDRAW;	//윈도우 그리기 방식 설정 ( 사이즈가 변경될때 화면갱신 CS_HREDRAW | CS_VREDRAW )

	//위에서 작성한 윈도우 클레스정보 등록
	RegisterClass( &WndClass );

	//윈도우 생성
	HWND hWnd = CreateWindow(
		windowName.c_str(),
		windowName.c_str(),
		WS_OVERLAPPEDWINDOW,
		X,
		Y,
		WIDTH,
		HEIGHT,
		GetDesktopWindow(),
		NULL,
		hInstance,
		NULL
		);

	//윈도우를 정확한 작업영역 크기로 맞춘다
	RECT rcClient = { 0, 0, WIDTH, HEIGHT};
	AdjustWindowRect( &rcClient, WS_OVERLAPPEDWINDOW, FALSE );	//rcClient 크기를 작업 영영으로 할 윈도우 크기를 rcClient 에 대입되어 나온다.

	const int width = rcClient.right - rcClient.left;
	const int height = rcClient.bottom - rcClient.top;
	const int screenCX = GetSystemMetrics(SM_CXSCREEN) * (dualMonitor? 2 : 1);
	const int screenCY = GetSystemMetrics(SM_CYSCREEN);
	const int x = screenCX/2 - width/2;
	int y = screenCY/2 - height/2;

	const float hRate = (float)height / (float)screenCY;
	if (hRate > 0.95f)
		y = 0;

	//윈도우 크기와 윈도우 위치를 바꾸어준다.
	SetWindowPos( hWnd, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE );
	MoveWindow(hWnd, x, y, width, height, TRUE);

	ShowWindow( hWnd, nCmdShow );
	
	return hWnd;
}
