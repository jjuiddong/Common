
// CPlotWindow, CMultPlotWindow 에서 공통으로 쓰이는 것들을 선언한다.
#pragma once


namespace plot
{

	enum MODE { NORMAL, SPLINE, CMD,};

	struct SPlotInfo
	{
		float xRange;
		float yRange;
		float xVisibleRange;
		float yVisibleRange;
		DWORD flags;
		string scanString;
		string name;
		MODE mode;
		int lineWidth;
		int timeLine;
		int vectorSize;
		int plotCount;
		float centerY;
	};

}
