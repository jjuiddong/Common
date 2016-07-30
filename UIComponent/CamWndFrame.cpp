// CamWndFrame.cpp : implementation file
//

#include "stdafx.h"
//#include "LEDDetector.h"
#include "CamWndFrame.h"


// CCamWndFrame
CCamWndFrame::CCamWndFrame()
	: m_camWindow(NULL)
{

}

CCamWndFrame::~CCamWndFrame()
{
}


BEGIN_MESSAGE_MAP(CCamWndFrame, CMiniFrameWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCamWndFrame message handlers
BOOL CCamWndFrame::Init(const CString &windowName, const CRect &rect)
{
	if (Create(NULL, windowName, WS_VISIBLE | WS_CAPTION | WS_POPUPWINDOW | WS_THICKFRAME, rect))
	{
		ShowWindow(SW_SHOW);
		return TRUE;
	}

	return FALSE;
}


BOOL CCamWndFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
	const RECT& rect, CWnd* pParentWnd, LPCTSTR lpszMenuName, DWORD dwExStyle, CCreateContext* pContext)
{
	if (CMiniFrameWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd))
	{
		CRect r(0, 0, rect.right - rect.left, rect.bottom - rect.top);
		m_camWindow = new CCamWindow();
		m_camWindow->Create(NULL, L"CamWindow", WS_VISIBLE | WS_CHILDWINDOW, r, this, 1001110);
		m_camWindow->SetScrollSizes(MM_TEXT, CSize(100, 100));
		m_camWindow->ShowWindow(SW_SHOW);
		return TRUE;
	}

	return FALSE;
}


void CCamWndFrame::OnSize(UINT nType, int cx, int cy)
{
	CMiniFrameWnd::OnSize(nType, cx, cy);

	if (m_camWindow)
		m_camWindow->MoveWindow(CRect(0, 0, cx, cy));
}

