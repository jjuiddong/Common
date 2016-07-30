#include "stdafx.h"
#include "CamWindow.h"
//#include "opencvgdiplus.h"


CCamWindow::CCamWindow() 
	: m_bitmap(NULL)
	, m_lastRenderTime(0)
	, m_imageSize(0,0)
	, m_isStartRect(false)
	, m_funcThisPtr(NULL)
{
	SetScrollSizes(MM_TEXT, CSize(100, 100));
}

CCamWindow::~CCamWindow()
{
	SAFE_DELETE(m_bitmap);
}


BEGIN_MESSAGE_MAP(CCamWindow, CScrollView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void CCamWindow::OnDraw(CDC* pDC)
{
	CRect r;
	GetClientRect(r);
	Gdiplus::Graphics g(*pDC);
	g.DrawImage(m_bitmap, Gdiplus::Rect(0, 0, r.right, r.bottom));
}


BOOL CCamWindow::OnEraseBkgnd(CDC* pDC)
{
	//return CScrollView::OnEraseBkgnd(pDC);
	return TRUE;
}


void CCamWindow::Render(IplImage *image)
{
	//------------------------------------------------
	const int t = timeGetTime();
	if (t - m_lastRenderTime < 33) // 30 frame
		return;
	m_lastRenderTime = t;
	//------------------------------------------------

	if (!m_bitmap)
		m_bitmap = IplImageToBitmap(image);
	else
		IplImageToBitmap(image, m_bitmap);

	// Render
	CRect r;
	GetClientRect(r);

	CDC *pDC = GetDC();
	Gdiplus::Graphics g(*pDC);
	g.DrawImage(m_bitmap, Gdiplus::Rect(0, 0, r.right, r.bottom));
	ReleaseDC(pDC);
}


void CCamWindow::Render(cv::Mat &image)
{
	using namespace Gdiplus;

	RET(image.empty());

	//------------------------------------------------
	const int t = timeGetTime();
	if (m_lastRenderTime == 0)
		m_lastRenderTime = t;
	else if (t - m_lastRenderTime < 33) // 30 frame
		return;
	m_lastRenderTime = t;
	//------------------------------------------------

	cv::Size size = image.size();
	if (m_imageSize == size)
	{
		CGdiPlus::CopyMatToBmp(image, m_bitmap);
	}
	else
	{
		SAFE_DELETE(m_bitmap);
		m_bitmap = CGdiPlus::CopyMatToBmp(image);
	}

	CRect r;
	GetClientRect(r);
	CDC *pDC = GetDC();
	Gdiplus::Graphics g(*pDC);
	g.DrawImage(m_bitmap, Gdiplus::Rect(0, 0, r.right, r.bottom));

	if (m_isStartRect && m_isClick)
	{
		Gdiplus::Pen pen(Gdiplus::Color(255, 255, 255));
		g.DrawRectangle(&pen, Gdiplus::Rect(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height()));
	}

	if (m_isStartRect)
	{
		Gdiplus::Font font(&FontFamily(L"Arial"), 12);
		SolidBrush brush(Color::White);

		Pen pen(Color(255, 255, 255));
		for (int i = 0; i < 24; ++i)
		{
			g.DrawLine(&pen, Gdiplus::Point(0, i*20), Gdiplus::Point(r.right, i*20));
			g.DrawString(common::formatw("%d", i+1).c_str(), -1, &font, PointF(0, i*20.f), &brush);
		}
		for (int i = 0; i < 32; ++i)
		{
			g.DrawLine(&pen, Gdiplus::Point(i * 32,0), Gdiplus::Point(i * 32, r.bottom));
			g.DrawString(common::formatw("%d", i + 1).c_str(), -1, &font, PointF(i*32.f,0), &brush);
		}
	}

	ReleaseDC(pDC);
}


void CCamWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_clickPos = point;
	m_rect.SetRectEmpty();
	m_isClick = true;
	CScrollView::OnLButtonDown(nFlags, point);
}


void CCamWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_isStartRect)
	{
		m_isStartRect = false;
		m_isClick = false;
		if (m_funcThisPtr) // class member function call
			(m_funcThisPtr->*m_memberFunc)(m_rect);
	}

	CScrollView::OnLButtonUp(nFlags, point);
}


void CCamWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	m_rect = CRect(m_clickPos.x, m_clickPos.y, point.x, point.y);
	m_rect.NormalizeRect();

	CScrollView::OnMouseMove(nFlags, point);
}


// Rect 가 완성된후 멤버 함수를 호출한다.
void CCamWindow::SetRect(void (CWnd::*func)(const CRect &out), CWnd *ptr)
{
	m_isStartRect = true;
	m_isClick = false;
	m_memberFunc = func;
	m_funcThisPtr = ptr;
}
