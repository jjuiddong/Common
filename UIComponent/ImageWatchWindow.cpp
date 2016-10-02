
#include "stdafx.h"
#include "ImageWatchWindow.h"


CImageWatchWindow::CImageWatchWindow()
	: m_bitmap(NULL)
	, m_memBmp(NULL)
	, m_lastRenderTime(0)
	, m_imageBytesSize(0)
	, m_dragImage(true)
	, m_blackPen(Gdiplus::Color(0, 0, 0))
	, m_smallFont(Gdiplus::FontFamily::GenericSerif(), 10, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel)
	, m_mediumFont(Gdiplus::FontFamily::GenericSerif(), 15, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel)
	, m_blackBrush(Gdiplus::Color(0, 0, 0))
	, m_whitePen(Gdiplus::Color(255, 255, 255))
	, m_whiteBrush(Gdiplus::Color(255, 255, 255))
	, m_grayBrush(Gdiplus::Color(200, 200, 200))
	, m_gray2Brush(Gdiplus::Color(100, 100, 100))
	, m_isShowGrid(TRUE)
	, m_isShowText(TRUE)
	, m_isShowTextBg(TRUE)
	, m_isShowWhite(TRUE)
{
	SetScrollSizes(MM_TEXT, CSize(100, 100));
}

CImageWatchWindow::~CImageWatchWindow()
{
	SAFE_DELETE(m_bitmap);
	SAFE_DELETE(m_memBmp);
}


BEGIN_MESSAGE_MAP(CImageWatchWindow, CScrollView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


void CImageWatchWindow::OnDraw(CDC* pDC)
{
	RenderZoom(pDC);
}

BOOL CImageWatchWindow::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CImageWatchWindow::ShowGrid(const BOOL show)
{ 
	m_isShowGrid = show; 
}
void CImageWatchWindow::ShowText(const BOOL show)
{ 
	m_isShowText = show; 
}
void CImageWatchWindow::ShowTextBackground(const BOOL show)
{
	m_isShowTextBg = show;
}
void CImageWatchWindow::ShowBlackandWhite(const BOOL isWhite)
{
	m_isShowWhite = isWhite;
}


// image setting, and render
void CImageWatchWindow::Render(cv::Mat &image)
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
	int size = image.total() * image.elemSize();
	if (m_imageBytesSize == size)
	{
		CGdiPlus::CopyMatToBmp(image, m_bitmap);
	}
	else
	{
		SAFE_DELETE(m_bitmap);
		m_bitmap = CGdiPlus::CopyMatToBmp(image);
		m_imageBytesSize = size;
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

	ReleaseDC(pDC);
}


// zoom in/out rendering
// double buffering
void CImageWatchWindow::RenderZoom(CDC* pDC)
{
	using namespace Gdiplus;
	RET(!m_bitmap);

	CRect cr;
	GetClientRect(cr);
	if (!m_memBmp || (m_memBmp->GetWidth() != cr.Width()) || (m_memBmp->GetHeight() != cr.Height()))
	{
		delete m_memBmp;
		m_memBmp = new Gdiplus::Bitmap(cr.Width(), cr.Height());
	}

	Gdiplus::Graphics *g = Gdiplus::Graphics::FromImage(m_memBmp);
	g->SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor); // no interpolation
	g->SetSmoothingMode(Gdiplus::SmoothingModeNone);
	g->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf); // ignore pixel coord.xy / 2

	const Gdiplus::Point scrollPos(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	const float zoomRateX = (float)m_bound.Width / (float)m_bitmap->GetWidth();
	const float zoomRateY = (float)m_bound.Height / (float)m_bitmap->GetHeight();

	g->DrawImage(m_bitmap,
		Gdiplus::RectF(0, 0, (float)cr.Width(), (float)cr.Height()),
		scrollPos.X / zoomRateX, scrollPos.Y / zoomRateY, cr.Width() / zoomRateX, cr.Height() / zoomRateY,
		Gdiplus::UnitPixel);

	if ((zoomRateX > 20.f) && (zoomRateY > 20.f))
	{
		const int incX = (int)zoomRateX;
		const int incY = (int)zoomRateY;
		int x = incX - (scrollPos.X % incX);
		int y = incY - (scrollPos.Y % incY);

		// render grid
		if (m_isShowGrid)
		{
			while (x < cr.Width())
			{
				g->DrawLine(m_isShowWhite? &m_whitePen : &m_blackPen, Gdiplus::Point(x, 0),
					Gdiplus::Point(x, cr.Height()));
				x += incX;
			}
			while (y < cr.Height())
			{
				g->DrawLine(m_isShowWhite ? &m_whitePen : &m_blackPen, Gdiplus::Point(0, y),
					Gdiplus::Point(cr.Width(), y));
				y += incY;
			}
		}

		// render text
		if (m_isShowText)
		{
			x = incX - (scrollPos.X % incX) - incX;
			for (; x < cr.Width(); x += incX)
			{
				const int xidx = (x + scrollPos.X) / incX;
				if (xidx < 0)
					continue;

				y = incY - (scrollPos.Y % incY) - incY;
				for (; y < cr.Height(); y += incY)
				{
					const int yidx = (y + scrollPos.Y) / incY;
					if (yidx < 0)
						continue;

					uchar *p = m_image.data + (xidx*m_image.elemSize()) + (m_image.step[0] * yidx);
					const Gdiplus::Point center(incX / 2 - 10, incY / 2 - 16);

					// render pixel data
					for (int i = 0; i < m_image.channels(); ++i)
					{
						switch (m_image.depth())
						{
						case CV_8U:
						case CV_8S:
							// render text background
							if (m_isShowTextBg)
								g->FillRectangle(m_isShowWhite ? &m_gray2Brush : &m_grayBrush, Gdiplus::RectF((float)(x + center.X), (float)(y + (i * 10)  + center.Y), 20, 12));

							const wstring str = common::formatw("%d", *p);
							const Gdiplus::RectF rt((float)(x + center.X), (float)(y + (i * 10) + center.Y), 20,20);
							g->DrawString(str.c_str(), str.length(), &m_smallFont,
								rt, Gdiplus::StringFormat::GenericDefault(), m_isShowWhite? &m_whiteBrush : &m_blackBrush);
							++p;
							break;
						}
					}
				}
			}
		}
	}

	Gdiplus::Graphics graphics(*pDC);
	graphics.DrawImage(m_memBmp, scrollPos.X, scrollPos.Y);
	delete g;
}


void CImageWatchWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	SetCapture();
	m_isClick = true;
	m_clickPos = point;
	CScrollView::OnLButtonDown(nFlags, point);
}


void CImageWatchWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	if (m_dragImage)
	{
		m_isClick = false;
	}
	CScrollView::OnLButtonUp(nFlags, point);
}


void CImageWatchWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_dragImage && m_isClick)
	{
		CPoint diff = point - m_clickPos;
		m_clickPos = point;
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - diff.y);
		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) - diff.x);
		InvalidateRect(NULL);
	}

	// image pixel information
	if (m_bitmap)
	{
		const Gdiplus::Point scrollPos(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
		const float zoomRateX = (float)m_bound.Width / (float)m_bitmap->GetWidth();
		const float zoomRateY = (float)m_bound.Height / (float)m_bitmap->GetHeight();
		int xidx = (int)((scrollPos.X + point.x) / zoomRateX);
		int yidx = (int)((scrollPos.Y + point.y) / zoomRateY);
		xidx = MAX(0, MIN(m_image.cols - 1, xidx));
		yidx = MAX(0, MIN(m_image.rows - 1, yidx));

		std::stringstream ss;
		uchar *p = m_image.data + (xidx*m_image.elemSize()) + (m_image.step[0] * yidx);
		for (int i = 0; i < m_image.channels(); ++i)
		{
			switch (m_image.depth())
			{
			case CV_8U:
			case CV_8S:
				ss << (int)*p << ", ";
				++p;
				break;
			}
		}

		string info = ss.str();
		if (info.size() > 2) // remove ", "
		{
			info.pop_back();
			info.pop_back();
		}
	
		m_infoStr = format("pt = (%d, %d), pixel = (%s)", xidx, yidx, info.c_str());
	}

	CScrollView::OnMouseMove(nFlags, point);
}


BOOL CImageWatchWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

