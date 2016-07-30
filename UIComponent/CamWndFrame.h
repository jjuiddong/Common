#pragma once

#include "CamWindow.h"

// CCamWndFrame frame
class CCamWindow;

class CCamWndFrame : public CMiniFrameWnd
{
public:
	CCamWndFrame();
	virtual ~CCamWndFrame();

	BOOL Init(const CString &windowName, const CRect &rect);
	CCamWindow *m_camWindow;


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = NULL, LPCTSTR lpszMenuName = NULL, DWORD dwExStyle = 0, CCreateContext* pContext = NULL);
};

