// DockablePaneBase.cpp : implementation file
//

#include "stdafx.h"
#include "DockablePaneBase.h"
#include "DockablePaneChildView.h"
#include "controller.h"


// CDockablePaneBase2
CDockablePaneBase2::CDockablePaneBase2() :
m_dlg(NULL)
{
}

CDockablePaneBase2::CDockablePaneBase2(CDockablePaneChildView2 *childView, UINT resourceID)
	: m_dlg(childView)
	, m_dlgResourceID(resourceID)
{
}

CDockablePaneBase2::~CDockablePaneBase2()
{
	SAFE_DELETE(m_dlg);
}


BEGIN_MESSAGE_MAP(CDockablePaneBase2, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEACTIVATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDockablePaneBase2 message handlers

int CDockablePaneBase2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	//cController::Get()->GetSerialComm().AddObserver(this);

	if (m_dlg)
	{
		m_dlg->Create(m_dlgResourceID, this);
		m_dlg->ShowWindow(SW_SHOW);
	}

	return 0;
}


// 윈도우 크기가 바뀌면, 자식 뷰도 같이 바뀐다.
void CDockablePaneBase2::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (m_dlg && GetSafeHwnd())
	{
		m_dlg->MoveWindow(CRect(0, 0, cx, cy));
	}
}


// 메뉴창 안뜨게 함.
void CDockablePaneBase2::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// nothing~
}


// CDockablePane 자식으로 CView가 있을 때, OnMouseActivate() 함수에서 죽는문제를 해결하기 위한 코드다.
// https://social.msdn.microsoft.com/Forums/en-US/e496c4da-221e-4c12-88d0-8b351a3a8986/cdockablepane-assertion-error-on-onmouseactivate?forum=vcgeneral
int CDockablePaneBase2::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	CFrameWnd* pParentFrame = GetParentFrame();
	if ((pParentFrame == pDesktopWnd) || (pDesktopWnd->IsChild(pParentFrame)))
	{
		return CDockablePane::OnMouseActivate(pDesktopWnd, nHitTest, message);
	}
	return MA_NOACTIVATE;
}


BOOL CDockablePaneBase2::OnEraseBkgnd(CDC* pDC)
{
	//return __super::OnEraseBkgnd(pDC);
	return TRUE;
}
