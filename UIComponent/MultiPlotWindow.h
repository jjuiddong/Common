//
// CPlotWindow 를 여러개 출력하는 윈도우 클래스.
//
// Plot Script
//
// plot%d = 0 0 0 0 
// string%d = %f
// name%d = name
// mode%d = spline, normal, plot
//					spline : 스플라인 곡선 출력
//					normal : 일반 그래프 출력
//					cmd : 명령어를 처리한다.
//						- @stop : 그래프 출력 멈춤
//
// linewidth%d = 2
// timeline%d = 1000 (milliseconds 단위) 화면에 출력할 x축 타임라인 시간 길이
// vector%d  = 100		미리 메모리를 잡을 벡터 크기 (아주 큰 벡터크기가 필요할 때 쓰인다.)
//
//
#pragma once

#include "PlotGlobal.h"


class CPlotWindow;
class CMultiPlotWindow : public CScrollView
{
public:
	CMultiPlotWindow();
	virtual ~CMultiPlotWindow();

	void SetString(const char *str, const int plotIndex=0);
	void SetString(const float t, const char *str, const int plotIndex = 0);
	void SetXY(const int plotIndex, const float x, const float y, const int graphIndex);
	void SetY(const int plotIndex, const float y, const int graphIndex);
	void DrawGraph(const float deltaSeconds, const bool autoSet=true);
	void ProcessPlotCommand(const CString &str, const int plotCount = 1);
	void SetFixedWidthMode(const bool isFixedWidth);


protected:
	struct SPlot
	{
		CPlotWindow *wnd;
		string scanString;
	};

	vector<SPlot> m_plotWindows;
	int m_plotWindowCount;

	vector<plot::SPlotInfo> m_PlotInfos;



protected:
	CPlotWindow* CreatePlotWindow(const int plotIndex);
	bool ParsePlotInfo(const int plotIndex, const wstring &str, plot::SPlotInfo &out);
	wstring ParseKeyValue(const wstring &str, const wstring &key);
	void CalcGraphWindowSize();


public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
