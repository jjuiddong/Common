
#pragma once


#include "gamemain/GameMain.h"

#pragma comment( lib, "winmm.lib" )


// 프레임워크 초기화. 
// WinMain 을 실행한다.
#define INIT_FRAMEWORK(frameWorkClassName) \
	framework::cGameMain* framework::CreateFrameWork()\
	{\
		return new frameWorkClassName();\
	}\
	\
	int APIENTRY WinMain(HINSTANCE hInstance, \
		HINSTANCE hPrevInstance, \
		LPSTR lpCmdLine, \
		int nCmdShow)\
	{\
		framework::FrameWorkWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);\
	}


#define INIT_FRAMEWORK2(frameWorkClassName, dualMonitor) \
	framework::cGameMain* framework::CreateFrameWork()\
	{\
		return new frameWorkClassName();\
	}\
	\
	int APIENTRY WinMain(HINSTANCE hInstance, \
		HINSTANCE hPrevInstance, \
		LPSTR lpCmdLine, \
		int nCmdShow)\
	{\
		framework::FrameWorkWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow, dualMonitor);\
	}


#define INIT_FRAMEWORK3(frameWorkClassName) \
	framework::cGameMain2* framework::CreateFrameWork2()\
	{\
		return new frameWorkClassName();\
	}\
	\
	int APIENTRY WinMain(HINSTANCE hInstance, \
		HINSTANCE hPrevInstance, \
		LPSTR lpCmdLine, \
		int nCmdShow)\
	{\
		framework::FrameWorkWinMain2(hInstance, hPrevInstance, lpCmdLine, nCmdShow);\
	}



// Include
#include "ui/uidef.h"
#include "ui/event.h"
#include "ui/eventhandler.h"
#include "ui/window2.h"
#include "ui/button.h"

#include "window/scene.h"

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_dx9.h"

// SFML
#include "external/sfml/include/SFML/Window.hpp"
