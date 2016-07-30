#pragma once

#include "UpdateObserver.h"
#include "UDPObserver.h"


// CDockablePaneChildView2 dialog

class CDockablePaneChildView2 : public CDialogEx
							 , public iUpdateObserver
{
public:
	CDockablePaneChildView2(UINT id, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDockablePaneChildView2();

	virtual void Update(const float deltaSeconds) override {}
	virtual void UpdateConfig(bool IsSaveAndValidate=true) {}
	virtual void Start() {}
	virtual void Stop() {}


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
