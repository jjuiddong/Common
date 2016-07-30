#pragma once

#include "UpdateObserver.h"
#include "UDPObserver.h"


// CDockablePaneChildView dialog

class CDockablePaneChildView : public CDialogEx
							 , public iUpdateObserver
							 , public iUDPObserver
							 , public iSerialObserver
{
public:
	CDockablePaneChildView(UINT id, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDockablePaneChildView();

	virtual void UpdateSerial(char *buffer) override {}
	virtual void Update(const float deltaSeconds) override {}
	virtual void UpdateUDP(const char *buffer, const int bufferLen) override {}
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
