
#include "stdafx.h"
#include "ImageFrameWnd.h"

BEGIN_MESSAGE_MAP(CImageFrameWnd::cImageView, CView)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CImageFrameWnd2
CImageFrameWnd::CImageFrameWnd() 
	: m_isAutoSize(false)
	, m_imageView(NULL)
{
}

CImageFrameWnd::~CImageFrameWnd()
{
}


BEGIN_MESSAGE_MAP(CImageFrameWnd, CMiniFrameWnd)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CImageFrameWnd2 message handlers

bool CImageFrameWnd::Init(const string &windowName, const string &fileName, const CRect &initialRect, 
	CWnd *parentWnd, const bool isAutoSize)
{
	Initialize(windowName, initialRect, parentWnd, isAutoSize);
	ShowImage(fileName);
	return true;
}


bool CImageFrameWnd::Init(const string &windowName, const cv::Mat &img, const CRect &initialRect, 
	CWnd *parentWnd, const bool isAutoSize)
{
	Initialize(windowName, initialRect, parentWnd, isAutoSize);
	ShowImage(img);
	return true;
}


// 윈도우 초기화, 
void CImageFrameWnd::Initialize(const string &windowName, const CRect &initialRect, CWnd *parentWnd, const bool isAutoSize)
{
	const CString strClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, 0, (HBRUSH)(COLOR_WINDOW + 1));
	CreateEx(0, strClass,
		str2wstr(windowName).c_str(),
		WS_POPUP | WS_CAPTION | WS_SYSMENU 
		| WS_MAXIMIZEBOX // to use double click full screen window
		//MFS_BLOCKSYSMENU
		,
		initialRect);

	CRect cr;
	GetClientRect(cr);
	m_imageView = new cImageView();
	m_imageView->Init(this, cr);
	m_imageView->ShowWindow(SW_SHOW);

	m_isAutoSize = isAutoSize;
	m_windowName = windowName;

	// TopMost
	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}


bool CImageFrameWnd::ShowImage(const string &fileName)
{
	SetWindowText(str2wstr(m_windowName + " - " + fileName).c_str());
	if (!m_imageView->ShowImage(fileName))
		return false;
	ReCalcWindowSize();
	return true;
}


bool CImageFrameWnd::ShowImage(const cv::Mat &img)
{
	SetWindowText(str2wstr(m_windowName).c_str());
	if (!m_imageView->ShowImage(img))
		return false;
	ReCalcWindowSize();
	return true;
}


void CImageFrameWnd::ReCalcWindowSize()
{
	if (m_isAutoSize)
	{
		const int sideEdgeSize = 20;
		const int captionSize = 40;
		const int w = MAX(300, (m_imageView->m_image)? m_imageView->m_image->GetWidth() + sideEdgeSize : 0);
		const int h = MAX(100, (m_imageView->m_image)? m_imageView->m_image->GetHeight() + captionSize : 0);
		CRect wr;
		GetWindowRect(wr);
		MoveWindow(CRect(wr.left, wr.top, wr.left + w, wr.top + h));
	}
}


void CImageFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CMiniFrameWnd::OnSize(nType, cx, cy);
	if (m_imageView)
	{
		m_imageView->MoveWindow(CRect(0, 0, cx, cy));
	}	
}


void CImageFrameWnd::OnClose()
{
	ShowWindow(SW_HIDE);
}

void CImageFrameWnd::SetAutoSize(const bool isAutoSize)
{
	m_isAutoSize = isAutoSize;
}


BOOL CImageFrameWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
