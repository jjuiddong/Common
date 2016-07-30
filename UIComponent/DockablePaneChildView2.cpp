// DockablePaneChildView.cpp : implementation file
//

#include "stdafx.h"
#include "DockablePaneChildView2.h"
#include "afxdialogex.h"


// CDockablePaneChildView2 dialog
CDockablePaneChildView2::CDockablePaneChildView2(UINT id, CWnd* pParent /*=NULL*/)
	: CDialogEx(id, pParent)
{
}

CDockablePaneChildView2::~CDockablePaneChildView2()
{
}

void CDockablePaneChildView2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDockablePaneChildView2, CDialogEx)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDockablePaneChildView2 message handlers
BOOL CDockablePaneChildView2::OnInitDialog()
{
	__super::OnInitDialog();

 	cController2::Get()->AddUpdateObserver(this);

	return TRUE;
}


BOOL CDockablePaneChildView2::OnEraseBkgnd(CDC* pDC)
{
	return __super::OnEraseBkgnd(pDC);
	//return TRUE;
}
