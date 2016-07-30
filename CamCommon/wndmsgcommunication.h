#pragma once


class cWndMsgCommunication
{
public:
	cWndMsgCommunication();
	virtual ~cWndMsgCommunication();

	bool Init();
	void SendData(const sSharedData &data);

	HWND m_hWnd;
};
