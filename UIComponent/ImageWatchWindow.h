//
// 2016-10-02, jjuiddong
// VisualStudio Extension Module - ImageWatch 와 비슷한 기능을 한다.
//
//
#pragma once

#include "afxwin.h"


class CImageWatchWindow : public CScrollView
{
public:
	CImageWatchWindow();
	virtual ~CImageWatchWindow();

	void Render(cv::Mat &image);
	void ShowGrid(const BOOL show);
	void ShowText(const BOOL show);
	void ShowTextBackground(const BOOL show);
	void ShowBlackandWhite(const BOOL isWhite);


protected:
	virtual void OnDraw(CDC* pDC);
	void RenderZoom(CDC* pDC);


public:
	cv::Mat m_image;
	Gdiplus::Bitmap *m_bitmap;
	int m_lastRenderTime;
	BOOL m_isShowGrid;
	BOOL m_isShowText;
	BOOL m_isShowTextBg;
	BOOL m_isShowWhite;

	int m_imageBytesSize;
	Gdiplus::Rect m_bound;
	string m_infoStr;

	Gdiplus::Pen m_blackPen;
	Gdiplus::Pen m_whitePen;
	Gdiplus::SolidBrush m_blackBrush;
	Gdiplus::SolidBrush m_whiteBrush;
	Gdiplus::SolidBrush m_grayBrush;
	Gdiplus::SolidBrush m_gray2Brush;
	Gdiplus::Font m_smallFont;
	Gdiplus::Font m_mediumFont;

	// double buffering
	Gdiplus::Bitmap *m_memBmp;

	// rectangle draw
	bool m_dragImage;
	bool m_isClick;
	CPoint m_clickPos;

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

