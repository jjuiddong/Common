// PlotWindow.cpp : implementation file
//

#include "stdafx.h"
#include "PlotWindow.h"

using namespace plot;


COLORREF g_penColors[] = {
	RGB(255, 255, 0),
	RGB(0, 255, 0),
	RGB(0, 255, 255),
	RGB(255, 255, 255),
};


// CPlotWindow
CPlotWindow::CPlotWindow() :
	m_mode(NORMAL)
	, m_splineIncTime(0)
	, m_oldTime(0)
	, m_bmpSize(0,0)
	, m_isDrawPlot(true)
{
	m_blackBrush.CreateSolidBrush(RGB(0, 0, 0));
	m_gridPen1.CreatePen(0, 1, RGB(100, 100, 100));
	m_gridPen2.CreatePen(0, 2, RGB(100,100,100));

}

CPlotWindow::~CPlotWindow()
{
}


BEGIN_MESSAGE_MAP(CPlotWindow, CScrollView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CPlotWindow drawing

void CPlotWindow::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 10;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// 그래프 출력.
void CPlotWindow::OnDraw(CDC* pDC)
{
	CRect cr;
	GetClientRect(cr);


	CDC memDC;
	CBitmap *pOldBitmap;
	memDC.CreateCompatibleDC(pDC);
	if ((m_bmpSize.cx != cr.Width()) || (m_bmpSize.cy != cr.Height()))
	{
		m_Bitmap.DeleteObject();
		m_Bitmap.CreateCompatibleBitmap(pDC, cr.Width(), cr.Height());
		m_bmpSize = CSize(cr.Width(), cr.Height());
	}

	pOldBitmap = memDC.SelectObject(&m_Bitmap);
	//memDC.PatBlt(0, 0, rect.Width(), rect.Height(), WHITENESS); // 흰색으로 초기화
	// 메모리 DC에 그리기
	//DrawImage(&memDC);
	// 메모리 DC를 화면 DC에 고속 복사
	//pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	// 	memDC.SelectObject(pOldBitmap);
	// 	memDC.DeleteDC();
	// 	bitmap.DeleteObject(






	// background black
	memDC.SelectObject(m_blackBrush);
	memDC.Rectangle(cr);

	// draw x,y axis grid
	memDC.SelectObject(m_gridPen2); // center line width 2
	memDC.MoveTo(CPoint(0, cr.Height() / 2));
	memDC.LineTo(CPoint(cr.Width(), cr.Height() / 2));
	memDC.SelectObject(m_gridPen1);
	memDC.MoveTo(CPoint(0, cr.Height() / 4));
	memDC.LineTo(CPoint(cr.Width(), cr.Height() / 4));
	memDC.MoveTo(CPoint(0, cr.Height() * 3 / 4));
	memDC.LineTo(CPoint(cr.Width(), cr.Height() * 3 / 4));


	// draw time line grid
	float oneSecondsWidth = 0; // 1초당 픽셀 간격

	// 1초당 픽셀폭이 고정된 모드일 경우.
	if (m_isFixedPlot)
	{
		if (m_timeLine > 0)
		{
			// timeline값이 설정되어 있으면, 화면 전체크기를 몇초에 채울지를 설정한다.
			oneSecondsWidth = cr.Width() / (m_timeLine / 1000.f);
			// 			if (oneSecondsWidth <= 0)
			// 				oneSecondsWidth = 1;
		}
		else
		{
			// 1초당 픽셀 간격을 100으로 고정시킨다.
			oneSecondsWidth = 100.f;
		}

		if (oneSecondsWidth <= 0)
			return;

		const float TOTAL_DRAW_TIMELINE = (float)cr.Width() / (float)oneSecondsWidth;
		// 타임라인보다 1초 적게 출력해서, 그래프 끝에 여분을 둔다.
		m_plots[0].renderStartIndex = GetDrawStartIndex(0, m_plots[0].renderStartIndex, TOTAL_DRAW_TIMELINE - 1);
	}
	else
	{ // 화면에 출력할 그래프의 시간을 고정시킬 경우.
		// 한 화면에 5초의 그래프를 출력하게 한다.
		const float TOTAL_DRAW_TIMELINE = 5.f;
		oneSecondsWidth = (float)cr.Width() / TOTAL_DRAW_TIMELINE; // 1초당 픽셀 간격
		// 타임라인보다 1초 적게 출력해서, 그래프 끝에 여분을 둔다.
		m_plots[0].renderStartIndex = GetDrawStartIndex(0, m_plots[0].renderStartIndex, TOTAL_DRAW_TIMELINE - 1);
	}

	const float timeElapse = m_plots[0].xy[m_plots[0].renderStartIndex].x - m_startTime; // 지나간 시간만큼 timeline을 이동시킨다.
	const float renderStartX = m_plots[0].xy[m_plots[0].renderStartIndex].x;

	if (oneSecondsWidth == 0)
		return;

	// 차이가 나는 시간 간격만큼 픽셀을 계산한다. 1초 단위의 간격만 알면된다.
	int offsetX = 0;
	float verticalTime = 1;
	float verticalLineWidth = oneSecondsWidth;
	while ((oneSecondsWidth > 0) && (verticalLineWidth < 10))
	{
		verticalLineWidth *= 10.f;
		verticalTime *= 10;
	}

	if (verticalLineWidth >= 1.f)
		offsetX = ((int)(timeElapse * verticalLineWidth / verticalTime)) % (int)verticalLineWidth;
	const int timeLineCount = (int)(cr.Width() / verticalLineWidth);

	for (int i = 1; i <= timeLineCount + 1; ++i)
	{
		memDC.MoveTo(CPoint((int)((i * verticalLineWidth) - offsetX), 0));
		memDC.LineTo(CPoint((int)((i * verticalLineWidth) - offsetX), cr.Height()));
	}

	m_scaleY = (float)cr.Height() / (m_maxY - m_minY);
	m_scaleX = oneSecondsWidth / 1.f; // 1초 당 픽셀수 //(float)(cr.Width() - 50) / (float)m_plot.size();


	for (u_int i = 0; i < m_plots.size(); ++i)
	{
		sPlotData &plot = m_plots[i];

		// 1초당 픽셀폭이 고정된 모드일 경우.
		if (m_isFixedPlot)
		{
			if (m_timeLine > 0)
			{
				// timeline값이 설정되어 있으면, 화면 전체크기를 몇초에 채울지를 설정한다.
				oneSecondsWidth = cr.Width() / (m_timeLine / 1000.f);
// 				if (oneSecondsWidth <= 0)
// 					oneSecondsWidth = 1;
			}
			else
			{
				// 1초당 픽셀 간격을 100으로 고정시킨다.
				oneSecondsWidth = 100;
			}

			if (oneSecondsWidth <= 0)
				continue;

			const float TOTAL_DRAW_TIMELINE = (float)cr.Width() / (float)oneSecondsWidth;
			plot.renderStartIndex = GetDrawStartIndex(i, plot.renderStartIndex, TOTAL_DRAW_TIMELINE);
		}
		else
		{ // 화면에 출력할 그래프의 시간을 고정시킬 경우.
			const float TOTAL_DRAW_TIMELINE = 5.f; // 한 화면에 5초의 그래프를 출력하게 한다.
			oneSecondsWidth = (float)cr.Width() / TOTAL_DRAW_TIMELINE; // 1초당 픽셀 간격
			plot.renderStartIndex = GetDrawStartIndex(i, plot.renderStartIndex, TOTAL_DRAW_TIMELINE);
		}

		// draw graph
		memDC.SelectObject(m_plotPens[ i]);
		const int h = cr.Height();
		int lastX = 0;
		for (int i = plot.renderStartIndex; i != plot.tailIdx; i = ++i % plot.xy.size())
		{
			//const int x = (int)((plot.xy[i].first - plot.xy[plot.renderStartIndex].first) * m_scaleX) - 50;
			const int x = (int)((plot.xy[i].x - renderStartX) * m_scaleX);// -50;
			const int y = h - (int)((plot.xy[i].y - m_minY) * m_scaleY);
			lastX = x;

			if (i == plot.renderStartIndex)
				memDC.MoveTo(CPoint(x, y));
			else
				memDC.LineTo(CPoint(x, y));
		}


		// 정보 출력. 최대, 최소, 중간 값, 현재 값.
		memDC.SetBkMode(TRANSPARENT);
		memDC.SetTextColor(RGB(220, 220, 220));

		// 가장 마지막 위치에 현재 값을 출력한다.
		if (plot.headIdx != plot.tailIdx)
		{
			int y = h - (int)((plot.xy[(plot.tailIdx - 1) % plot.xy.size()].y - m_minY) * m_scaleY);
			if (cr.Height() < (y + 20)) {
				y -= 20;
			}
			else {
				y += 10;
			}

			CString strLastVal;
			strLastVal.Format(L"%f", plot.xy[(plot.tailIdx - 1) % plot.xy.size()].y);
			memDC.TextOutW(lastX - 70, y, strLastVal);
		}
	}


	CString strMaxY, strMinY, strCenterY;
	strMaxY.Format(L"%f", m_maxY);
	strMinY.Format(L"%f", m_minY);
	strCenterY.Format(L"%f", (m_maxY + m_minY) / 2.f);
	memDC.TextOutW(5, 0, strMaxY); // maximum y
	memDC.TextOutW(5, cr.Height() - 20, strMinY); // minimum y
	memDC.TextOutW(5, cr.Height() / 2 - 20, strCenterY); // center y
	memDC.TextOutW(cr.Width()/2, 0, m_name); // plot name

	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
}


// Plot을 Frame per Seconds에 맞춰 출력하게 한다.
void CPlotWindow::DrawPlot(const float deltaSeconds, const bool autoSet) //autoSet=true
{
	const float elapseTime = 0.03f; // 1초에 30프레임일 때, 시간 간격

	// 가장 최근에 업데이트 된 시간과 현재시간을 비교해서, 
	// 일정시간 이상 그래프가 업데이트가 되지 않았다면,
	// 가장 최근 값으로, 그래프 값을 추가한다.
	const float curT = timeGetTime() * 0.001f;
	if (autoSet && (curT - m_updateTime) > elapseTime) // 30프레임 주기로 그래프를 업데이트 한다.
	{
		// 가장 최근 값을 저장한다.
		for (u_int i = 0; i < m_plots.size(); ++i)
			SetPlotXY(curT, GetTailValue(i).y, i);
	}

	m_incSeconds += deltaSeconds;
	if (m_incSeconds > elapseTime) // 1초에 30프레임을 유지한다.
	{
		if (IsWindowVisible())
			InvalidateRect(NULL);
		m_incSeconds = 0;
	}
}


// CPlotWindow diagnostics

#ifdef _DEBUG
void CPlotWindow::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlotWindow::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlotWindow message handlers
// bool CPlotWindow::SetPlot(const float x_range, const float y_range,
// 	const float x_visble_range, const float y_visible_range, const DWORD flags,
// 	const int plotCount, const string &name, const MODE &mode, const int lineWidth)  
// 	// plotCount=1, name="", mode=NORMAL, lineWidth=1
// {
// 	m_xRange = x_range;
// 	m_yRange = y_range;
// 	m_xVisibleRange = x_visble_range;
// 	m_yVisibleRange = y_visible_range;
// 	m_flags = flags;
// 	m_mode = mode;
// 	m_name = common::str2wstr(name).c_str();
// 
// 	m_plots.resize(plotCount);
// 	for (u_int i = 0; i < m_plots.size(); ++i)
// 	{
// 		m_plots[i].xy.resize(2048, Vector2(0.f, 0.f));
// 		m_plots[i].headIdx = 0;
// 		m_plots[i].tailIdx = 0;
// 		m_plots[i].renderStartIndex = 0;
// 
// 		m_plots[i].splineTemp.resize(2048, Vector2(0.f, 0.f));
// 		m_plots[i].spHeadIdx = 0;
// 		m_plots[i].spTailIdx = 0;
// 	}
// 
// 	m_maxX = -FLT_MAX;
// 	m_minX = FLT_MAX;
// 
// 	if (0 == m_yRange)
// 	{
// 		m_maxY = -FLT_MAX;
// 		m_minY = FLT_MAX;
// 	}
// 	else
// 	{
// 		m_maxY = m_yRange/2;
// 		m_minY = -m_yRange/2;
// 	}
// 
// 	m_scaleY = 1.f;
// 	m_startTime = 0;
// 	m_updateTime = 0;
// 
// 	for (int i = 0; i < 4; ++i)
// 	{
// 		m_plotPens[i].DeleteObject();
// 		m_plotPens[i].CreatePen(0, lineWidth, g_penColors[i]);
// 	}
// 
// 	return true;
// }
bool CPlotWindow::SetPlot(const plot::SPlotInfo &info)
{
	m_isDrawPlot = true;

	m_xRange = info.xRange;
	m_yRange = info.yRange;
	m_xVisibleRange = info.xVisibleRange;
	m_yVisibleRange = info.yVisibleRange;
	m_flags = info.flags;
	m_mode = info.mode;
	m_name = common::str2wstr(info.name).c_str();
	m_timeLine = info.timeLine;
	m_vectorSize = info.vectorSize;
	m_centerY = info.centerY;

	m_plots.resize(info.plotCount);
	for (u_int i = 0; i < m_plots.size(); ++i)
	{
		m_plots[i].xy.resize((info.vectorSize == 0) ? 2048 : info.vectorSize, Vector2(0.f, 0.f));
		m_plots[i].headIdx = 0;
		m_plots[i].tailIdx = 0;
		m_plots[i].renderStartIndex = 0;

		m_plots[i].splineTemp.resize((info.vectorSize == 0) ? 2048 : info.vectorSize, Vector2(0.f, 0.f));
		m_plots[i].spHeadIdx = 0;
		m_plots[i].spTailIdx = 0;
	}

	m_maxX = -FLT_MAX;
	m_minX = FLT_MAX;

	if (0 == m_yRange)
	{
		m_maxY = -FLT_MAX;
		m_minY = FLT_MAX;
	}
	else
	{
		m_maxY = m_yRange / 2;
		m_minY = -m_yRange / 2;
	}

	m_scaleY = 1.f;
	m_startTime = 0;
	m_updateTime = 0;

	for (int i = 0; i < 4; ++i)
	{
		m_plotPens[i].DeleteObject();
		m_plotPens[i].CreatePen(0, info.lineWidth, g_penColors[i]);
	}

	return true;
}


// 그래프 정보 추가.
void CPlotWindow::SetPlotY(const float y, const int plotIndex)
{
	// X축은 시간축으로 한다. 
	const float t = timeGetTime() * 0.001f;
	const float deltaSeconds = (t - m_oldTime);
	m_oldTime = t;
	m_splineIncTime += deltaSeconds;

	sPlotData &plot = m_plots[plotIndex];

	if ((SPLINE == m_mode) && (plotIndex == 1))
	{
		if (m_splineIncTime < 0.1f)
			return;
		
		m_splineIncTime = 0;

		// 현재 값 저장
		const int size = plot.splineTemp.size();

		// Ring형 배열 구조, 추가
		plot.splineTemp[plot.spTailIdx] = Vector2(t, y);
		plot.spTailIdx = ++plot.spTailIdx % size;
		if (plot.spHeadIdx == plot.spTailIdx)
			plot.spHeadIdx = ++plot.spHeadIdx % size;

		const int indexGap = 1;

		// 가장 최근에 저장된 3개의 포인트를 찾는다.
		// Vector2(x,y)를 포함한 총 4개의 포인트로 spline 곡선을 만든다.
		// 현재 포인트와 그 전 포인트의 중간을 spline 곡선 알고리즘을 통해 계산한 후, 저장한다.
		const int idx4 = (plot.spTailIdx - 1 + size) % size;
		const int idx3 = (plot.spTailIdx - (indexGap*1 + 1) + size) % size;
		const int idx2 = (plot.spTailIdx - (indexGap*2 + 1) + size) % size;
		const int idx1 = (plot.spTailIdx - (indexGap*3 + 1) + size) % size;
		const int length = (plot.spTailIdx - plot.spHeadIdx + size) % size;
		if (length > (indexGap*3 + 1)) // 4개 이상 저장 되었을 때부터, spline연산을 시작한다.
		{
			const Vector2 p1 = plot.splineTemp[idx1];
			const Vector2 p2 = plot.splineTemp[idx2];
			const Vector2 p3 = plot.splineTemp[idx3];
			const Vector2 p4 = plot.splineTemp[idx4];

			float t = 0.0f;
			while (t < 1.f)
			{
				Vector2 p;
				D3DXVec2CatmullRom((D3DXVECTOR2*)&p,
					(D3DXVECTOR2*)&p1,
					(D3DXVECTOR2*)&p2,
					(D3DXVECTOR2*)&p3,
					(D3DXVECTOR2*)&p4, t);

				// insert spline curve b3
				SetPlotXY(p.x, p.y, plotIndex);

				t += .1f;
			}
		}
		else
		{
			SetPlotXY(t, y, plotIndex);
		}

	}
	else
	{
		SetPlotXY(t, y, plotIndex);
	}

}


// Ring형 배열 구조, 추가
void CPlotWindow::SetPlotXY(const float x, const float y, const int plotIndex) //plotIndex=0
{
	if (!m_isDrawPlot)
		return;

	if ((int)m_plots.size() <= plotIndex)
		return;

	sPlotData &plot = m_plots[plotIndex];
	plot.xy[plot.tailIdx] = Vector2(x, y);
	plot.tailIdx = ++plot.tailIdx % plot.xy.size();
	if (plot.headIdx == plot.tailIdx)
		plot.headIdx = ++plot.headIdx % plot.xy.size();

	m_maxX = MAX(x, m_maxX);
	m_minX = MIN(x, m_minX);
	m_maxY = MAX(y, m_maxY);
	m_minY = MIN(y, m_minY);

	if (m_centerY != FLT_MIN)
	{
		const float cy = MAX(abs(m_maxY - m_centerY), abs(m_minY - m_centerY));
		m_maxY = m_centerY + cy;
		m_minY = m_centerY - cy;		
	}

	const float t = timeGetTime() * 0.001f;
	m_updateTime = t; // 가장 최근 값이 바뀐 시간을 저장한다.

	if (m_startTime == 0) // 처음 그래프가 그려질 때, 초기화 된다.
		m_startTime = x;
}


void CPlotWindow::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
}


BOOL CPlotWindow::OnEraseBkgnd(CDC* pDC)
{
	//return CScrollView::OnEraseBkgnd(pDC); 백그라운드 화면을 갱신하지 않는다.
	return TRUE;
}


// 가장 먼저 저장된 값을 리턴 한다. (가장 오래된 값)
Vector2 CPlotWindow::GetHeadValue(const u_int plotIndex) const
{
	const sPlotData &plot = m_plots[plotIndex];

	if (plot.headIdx < plot.tailIdx)
	{
		return plot.xy[plot.headIdx];
	}
	else
	{
		const int idx = (plot.xy.size() + plot.headIdx - 1) % plot.xy.size();
		return plot.xy[idx];
	}
}


// 가장 나중에 저장 된 값을 리턴한다. (가장 최근 값)
Vector2 CPlotWindow::GetTailValue(const u_int plotIndex) const
{
	const sPlotData &plot = m_plots[plotIndex];
	const int idx = (plot.xy.size() + plot.tailIdx - 1) % plot.xy.size();
	return plot.xy[idx];
}


// 현재 plot index 부터 tail 까지 쫓아가면서 drawTimeRange 범위에 안에 드는 
// 첫 번째 plot index 를 찾아 리턴한다.
// 그래프가 정확히 drawTimeRange 만큼 출력하기 위한 함수다.
int CPlotWindow::GetDrawStartIndex(const u_int plotIndex, const int currentIndex, const float drawTimeRange)
{
	const float tailSeconds = GetTailValue(plotIndex).x;

	for (int i = currentIndex; i != m_plots[plotIndex].tailIdx; i = ++i % m_plots[plotIndex].xy.size())
	{
		const float totalDrawTime = tailSeconds - m_plots[plotIndex].xy[i].x;
		if (totalDrawTime < drawTimeRange)
			return i;
	}

	// 찾지 못했다면, 현재 인덱스를 리턴한다.
	return currentIndex;
}


// 그래프 출력을 멈춘다.
void CPlotWindow::Stop()
{
	m_isDrawPlot = false;
}


void CPlotWindow::Start()
{
	m_isDrawPlot = true;
}