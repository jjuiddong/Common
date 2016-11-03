#pragma once

#include "MultiPlotWindow.h"


// CPlotWindow view

class CPlotWindow : public CScrollView
{
public:
	CPlotWindow();
	virtual ~CPlotWindow();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	bool SetPlot(const plot::SPlotInfo &info);

	void SetPlotXY(const float x, const float y, const int plotIndex = 0);
	void SetPlotY(const float y, const int plotIndex=0);
	void SetMode(const plot::MODE &mode);
	void DrawPlot(const float deltaSeconds, const bool autoSet = true);
	void SetFixedWidthMode(const bool isFixedWidth);
	void Stop();
	void Start();


//protected:
public:
	// Plot variable
	bool m_isDrawPlot;

	float m_xRange;
	float m_yRange;
	float m_xVisibleRange;
	float m_yVisibleRange;
	DWORD m_flags;
	plot::MODE m_mode;
	CString m_name;

	float m_maxX;
	float m_minX;
	float m_maxY;
	float m_minY;
	float m_scaleY;
	float m_scaleX;
	int m_timeLine;
	int m_vectorSize;
	float m_centerY;

	// plot sync
	float m_updateTime = 0; // seconds
	float m_incSeconds = 0; // Frame per Seconds 를 맞추기 위해 증가된 시간을 저장한다.
	float m_startTime = 0; // 그래프가 그려지기 시작한 시간. 처음 SetPlotXY()에서 초기화 된다. Seconds 단위.

	// plot array
	struct sPlotData {
		vector<Vector2> xy;
		int headIdx = 0;
		int tailIdx = 0;
		int renderStartIndex = 0;

		// spline
		vector<Vector2> splineTemp; // 입력으로 들어온 값을 저장한다. spline곡선 연산이 적용된 값은 xy에 저장된다.
		int spHeadIdx = 0;
		int spTailIdx = 0;
	};

	vector<sPlotData> m_plots;

	bool m_isFixedPlot = false;
	CBrush m_blackBrush;
	CPen m_plotPens[4];
	CPen m_gridPen1; // line width 1
	CPen m_gridPen2; // line width 2
	CBitmap m_Bitmap; // double buffering
	CSize m_bmpSize;

	float m_splineIncTime;
	float m_oldTime;


protected:
 	Vector2 GetHeadValue(const u_int plotIndex = 0) const;
 	Vector2 GetTailValue(const u_int plotIndex = 0) const;
	int GetDrawStartIndex(const u_int plotIndex, const int currentIndex, const float drawTimeRange);

	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


inline void CPlotWindow::SetFixedWidthMode(const bool isFixedWidth) { m_isFixedPlot = isFixedWidth; }
inline void CPlotWindow::SetMode(const plot::MODE &mode) { m_mode = mode; }
