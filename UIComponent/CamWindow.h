#pragma once

#include "afxwin.h"


class CCamWindow : public CScrollView
{
public:
	CCamWindow();
	virtual ~CCamWindow();

	void Render(IplImage *image);
	void Render(cv::Mat &image);
	void SetRect(void (CWnd::*func)(const CRect &out), CWnd *ptr);


protected:
	virtual void OnDraw(CDC* pDC);


public:
	cv::Mat m_image;
	Gdiplus::Bitmap *m_bitmap;
	cv::Size m_imageSize;
	int m_lastRenderTime;
	Gdiplus::Rect m_bound;
	Gdiplus::Font m_Font;
	Gdiplus::SolidBrush m_Brush;
	Gdiplus::SolidBrush m_grayBrush;

	// double buffering
	Gdiplus::Bitmap *m_memBmp;

	// rectangle draw
	bool m_dragImage;
	bool m_isStartRect;
	bool m_isClick;
	CPoint m_clickPos;
	CRect m_rect;
	void (CWnd::*m_memberFunc)(const CRect &out);
	CWnd *m_funcThisPtr;

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

