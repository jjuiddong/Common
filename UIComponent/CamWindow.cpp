#include "stdafx.h"
#include "CamWindow.h"
//#include "opencvgdiplus.h"


CCamWindow::CCamWindow() 
	: m_bitmap(NULL)
	, m_memBmp(NULL)
	, m_lastRenderTime(0)
	, m_imageSize(0,0)
	, m_isStartRect(false)
	, m_funcThisPtr(NULL)
	, m_dragImage(true)
	, m_Font(Gdiplus::FontFamily::GenericSerif(), 10, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel)
	, m_Brush(Gdiplus::Color(0, 0, 0))
	, m_grayBrush(Gdiplus::Color(200, 200, 200))
{
	SetScrollSizes(MM_TEXT, CSize(100, 100));
}

CCamWindow::~CCamWindow()
{
	SAFE_DELETE(m_bitmap);
	SAFE_DELETE(m_memBmp);
}


BEGIN_MESSAGE_MAP(CCamWindow, CScrollView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


void CCamWindow::OnDraw(CDC* pDC)
{
	using namespace Gdiplus;
	RET(!m_bitmap);

	CRect cr;
	GetClientRect(cr);
// 	if (!m_memBmp || (m_memBmp->GetWidth() != m_bound.Width) || (m_memBmp->GetHeight() != m_bound.Height))
// 	{
// 		delete m_memBmp;
// 		m_memBmp = new Gdiplus::Bitmap(m_bound.Width, m_bound.Height);
// 	}
	if (!m_memBmp || (m_memBmp->GetWidth() != cr.Width()) || (m_memBmp->GetHeight() != cr.Height()))
	{
		delete m_memBmp;
		m_memBmp = new Gdiplus::Bitmap(cr.Width(), cr.Height());
	}

	//Graphics* pMemGraphics = Graphics::FromImage(pMemBitmap);

	//Gdiplus::Graphics g(*pDC);
	//Gdiplus::Graphics g(m_memBmp);
	Gdiplus::Graphics *g = Gdiplus::Graphics::FromImage(m_memBmp);
	g->SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
	g->SetSmoothingMode(Gdiplus::SmoothingModeNone);
	g->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

	Gdiplus::Point scrollPos(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));

	g->DrawImage(m_bitmap, m_bound);
	//g->DrawImage(m_bitmap, 0, 0, scrollPos.X, scrollPos.Y, cr.Width(), cr.Height(), UnitPixel);

	const float zoomRateX = (float)m_bound.Width / (float)m_bitmap->GetWidth();
	const float zoomRateY = (float)m_bound.Height / (float)m_bitmap->GetHeight();
	if ((zoomRateX > 20.f) && (zoomRateY > 20.f))
	{		
		const int incX = (int)zoomRateX;
		const int incY = (int)zoomRateY;
		int x = incX - (scrollPos.X % incX);
		int y = incY - (scrollPos.Y % incY);

		// grid
		Gdiplus::Pen pen(Gdiplus::Color(255, 255, 255));
		while (x < cr.Width())
		{
			g->DrawLine(&pen, Gdiplus::Point(x, 0) + scrollPos,
				Gdiplus::Point(x, cr.Height()) + scrollPos);
			x += incX;
		}
		while (y < cr.Height())
		{
			g->DrawLine(&pen, Gdiplus::Point(0, y) + scrollPos,
				Gdiplus::Point(cr.Width(), y) + scrollPos);
			y += incY;
		}

		x = incX - (scrollPos.X % incX);
		for (; x < cr.Width(); x += incX)
		{
			const int xidx = (x + scrollPos.X) / incX;
			y = incY - (scrollPos.Y % incY);
			for (; y < cr.Height(); y += incY)
			{
				const int yidx = (y + scrollPos.Y) / incY;
				uchar *p = m_image.data + (xidx*m_image.elemSize()) + (m_image.step[0] * yidx);
				Gdiplus::Point center(incX/2-10, incY/2-16);
				g->FillRectangle(&m_grayBrush, Gdiplus::RectF((float)(x + scrollPos.X + center.X), (float)(y + scrollPos.Y+ center.Y), 20, 32));

				for (int i = 0; i < m_image.channels(); ++i)
				{
					switch (m_image.depth())
					{
					case CV_8U:
					case CV_8S:
						wstring str = common::formatw("%d", *p);
						Gdiplus::RectF rt(
							(float)(x + scrollPos.X + center.X),
							(float)(y + scrollPos.Y + (i * 10) + center.Y),
							20,
							20);
						g->DrawString(str.c_str(), str.length(), &m_Font,
							rt, Gdiplus::StringFormat::GenericDefault(), &m_Brush);
						++p;
						break;
					}
				}
			}
		}
	}

	Gdiplus::Graphics graphics(*pDC);
	graphics.DrawImage(m_memBmp, 0, 0);

	delete g;
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

	m_image = image.clone();
	cv::Size size = image.size();
	if (m_imageSize == size)
	{
		CGdiPlus::CopyMatToBmp(image, m_bitmap);
	}
	else
	{
		SAFE_DELETE(m_bitmap);
		m_bitmap = CGdiPlus::CopyMatToBmp(image);
		m_imageSize = size;
	}

	CRect cr;
	GetClientRect(cr);
	if ((cr.Width() == 0) || (cr.Height() == 0))
		return;

	// 2의 배수가 되게 한다. 확대축소 시 깨짐 방지. (ceil() 올림함수)
	int rate = (int)ceil((double)m_bitmap->GetWidth() / (double)cr.Width());
	if (rate <= 0)
		rate = 1;
	else if (rate % 2)
		rate += 1;

	m_bound = Gdiplus::Rect(0, 0, m_bitmap->GetWidth() / rate, m_bitmap->GetHeight() / rate);

	CDC *pDC = GetDC();
	Gdiplus::Graphics g(*pDC);
	g.DrawImage(m_bitmap, Gdiplus::Rect(0, 0, m_bound.Width, m_bound.Height));

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
			g.DrawLine(&pen, Gdiplus::Point(0, i*20), Gdiplus::Point(cr.right, i*20));
			g.DrawString(common::formatw("%d", i+1).c_str(), -1, &font, PointF(0, i*20.f), &brush);
		}
		for (int i = 0; i < 32; ++i)
		{
			g.DrawLine(&pen, Gdiplus::Point(i * 32,0), Gdiplus::Point(i * 32, cr.bottom));
			g.DrawString(common::formatw("%d", i + 1).c_str(), -1, &font, PointF(i*32.f,0), &brush);
		}
	}

	ReleaseDC(pDC);
}


void CCamWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	SetCapture();
	m_clickPos = point;
	m_rect.SetRectEmpty();
	m_isClick = true;
	CScrollView::OnLButtonDown(nFlags, point);
}


void CCamWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if(m_isStartRect)
	{
		m_isStartRect = false;
		m_isClick = false;
		if (m_funcThisPtr) // class member function call
			(m_funcThisPtr->*m_memberFunc)(m_rect);
	}

	if (m_dragImage)
	{
		m_isClick = false;
	}

	CScrollView::OnLButtonUp(nFlags, point);
}


void CCamWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	m_rect = CRect(m_clickPos.x, m_clickPos.y, point.x, point.y);
	m_rect.NormalizeRect();

	if (m_dragImage && m_isClick)
	{
		CPoint diff = point - m_clickPos;
		m_clickPos = point;
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT)-diff.y);
		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ)-diff.x);
		InvalidateRect(NULL);
	}

	CScrollView::OnMouseMove(nFlags, point);
}


// Rect 가 완성된후 멤버 함수를 호출한다.
void CCamWindow::SetRect(void (CWnd::*func)(const CRect &out), CWnd *ptr)
{
	m_isStartRect = true;
	m_dragImage = false;
	m_isClick = false;
	m_memberFunc = func;
	m_funcThisPtr = ptr;
}


BOOL CCamWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CRect cr;
	GetClientRect(cr);
	// 축소 비율 제한
	if ((zDelta < 0) && (cr.Width() > m_bound.Width) && (cr.Height() > m_bound.Height))
	{
		SetScrollPos(SB_HORZ, 0);
		SetScrollPos(SB_VERT, 0);
		return TRUE;
	}

	// 최대 확대 비율 40x
	if ((zDelta > 0) && (m_bound.Width / m_bitmap->GetWidth()) > 127)
		return TRUE;

	ScreenToClient(&pt);
	CPoint curPos(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	curPos += pt;
	const CSize curSize(m_bound.Width, m_bound.Height);

	if (zDelta > 0)
	{
		m_bound.Width *= 2;
		m_bound.Height *= 2;
	}
	else
	{
		m_bound.Width /= 2;
		m_bound.Height /= 2;
	}

	SetScrollSizes(MM_TEXT, CSize(m_bound.Width, m_bound.Height));

	const CPoint target((int)(m_bound.Width * ((float)curPos.x / (float)curSize.cx)),
		(int)(m_bound.Height * ((float)curPos.y / (float)curSize.cy)));

	const int scrollX = MAX(0, target.x - pt.x);
	const int scrollY = MAX(0, target.y - pt.y);
 	SetScrollPos(SB_VERT, scrollY);
 	SetScrollPos(SB_HORZ, scrollX);

	InvalidateRect(NULL);
	return true;
}
