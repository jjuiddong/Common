//
// 2016-06-22, jjuiddong
// joypad 입력처리
// directx input sample code 참조.
//
// Hardware
//		- EXM AiR 
//
#pragma once

#include <dinput.h>
#include <dinputd.h>
#include <oleauto.h>


namespace motion
{

	class cJoyPad;
	struct XINPUT_DEVICE_NODE
	{
		DWORD dwVidPid;
		XINPUT_DEVICE_NODE* pNext;
	};

	struct DI_ENUM_CONTEXT
	{
		DIJOYCONFIG* pPreferredJoyCfg;
		bool bPreferredJoyCfgValid;
		cJoyPad *joypad;
	};


	class cJoyPad
	{
	public:
		cJoyPad();
		virtual ~cJoyPad();

		bool Init(HWND hWnd);
		void Update(const float deltaSeconds);
		void Clear();
		HRESULT SetupForIsXInputDevice();
		void CleanupForIsXInputDevice();
		bool IsXInputDevice(const GUID* pGuidProductFromDirectInput);


	public:
		LPDIRECTINPUT8 m_pDI;
		LPDIRECTINPUTDEVICE8 m_pJoystick;
		bool m_bFilterOutXinputDevices;
		XINPUT_DEVICE_NODE* m_pXInputDeviceList;

		// range = -1000 ~ +1000
		LONG m_axisX;
		LONG m_axisY;
		LONG m_axisZ;
		LONG m_axisRx;
		LONG m_axisRy;
		LONG m_axisRz;
		LONG m_axisH;
		LONG m_Hat;
	};

}
