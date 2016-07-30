#pragma once

#include "SerialObserver.h"

// CDockablePaneBase2

class CDockablePaneChildView2;
class CDockablePaneBase2 : public CDockablePane
{
public:
	CDockablePaneBase2();
	CDockablePaneBase2(CDockablePaneChildView2 *childView, UINT resourceID);
	virtual ~CDockablePaneBase2();

	void SetChildView(CDockablePaneChildView2 *dlg);
	CDockablePaneChildView2* GetChildView();


protected:
	UINT m_dlgResourceID = 0;
	CDockablePaneChildView2 *m_dlg = NULL;

	DECLARE_MESSAGE_MAP()


public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


inline void CDockablePaneBase2::SetChildView(CDockablePaneChildView2 *dlg) { m_dlg = dlg; }
inline CDockablePaneChildView2* CDockablePaneBase2::GetChildView() { return m_dlg; }
