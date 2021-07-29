//
// 2017-07-27, jjuiddong
// Upgrade Dx9 -> Dx11
//
//
#pragma once

#pragma warning(disable: 4996) // disable warning 'GetVersion()'

#include "gamemain/GameMain.h"

#pragma comment( lib, "winmm.lib" )


// 프레임워크 초기화. 
// WinMain 을 실행한다.
#define INIT_FRAMEWORK(frameWorkClassName) \
	framework::cGameMain* g_application=NULL;\
	framework::cGameMain* framework::CreateFrameWork()\
	{\
		g_application = new frameWorkClassName();\
        return g_application;\
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
	framework::cGameMain2 * g_application=NULL;\
	framework::cGameMain2* framework::CreateFrameWork2()\
	{\
		g_application = new frameWorkClassName();\
        return g_application;\
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
#include "window/utility.h"


// ImGui
#include "external/nodeeditor/imgui_node_editor.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_dx11.h"
// ImGui NodeEditor
#include "external/nodeeditor/ax/builders.h"
#include "external/nodeeditor/ax/widgets.h"


// SFML
#pragma warning(push)
//#pragma warning (disable: 4603) //warning C4603: '_WIN32_WINDOWS' : macro is not defined or definition is different after precompiled header use
#include "external/sfml/include/SFML/Window.hpp"
#pragma warning(pop)

#include "input/input.h"


//#include "window/utility.h"
#include "ui/dockwindow.h"
#include "ui/dockmanager.h"
#include "ui/renderwindow.h"
#include "gamemain/GameMain2.h"

#include "dbg/dbgmonitor.h"
#include "vprogeditor/vpl.h"

